#!/bin/sh

mv hdd.img hdd.img.bak

rm -f hdd.img 
sudo mkdir -p mnt

dd if=/dev/zero of=hdd.img count=40960 # 20MB
sfdisk hdd.img < sfdisk.layout
sfdisk -l hdd.img

sudo umount /dev/loop1
sudo losetup -o 512 --sizelimit 30965248 /dev/loop1 hdd.img
sudo mkfs.ext2 /dev/loop1

sudo mount /dev/loop1 mnt

sudo mkdir mnt/bin
sudo mkdir mnt/dev
sudo mknod mnt/dev/tty c 1 0
sudo mknod mnt/dev/kbd c 2 0
sudo mknod mnt/dev/hda c 8 0
sudo mknod mnt/dev/hda1 c 8 1
sudo cp ../userspace/init/init.elf mnt/bin/init
sudo cp ../userspace/hexdump/hexdump.elf mnt/bin/hexdump
sudo cp ../userspace/ushell/ushell.elf mnt/bin/ush

sudo umount mnt
sudo losetup -D
#sudo rm -rf mnt
