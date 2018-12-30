#!/bin/sh

mv hdd.img hdd.img.bak

rm -f hdd.img 
sudo mkdir -p mnt

bximage -mode=create -hd=30M -q hdd.img
sfdisk hdd.img < sfdisk.layout
sfdisk -l hdd.img

sudo losetup -o 512 --sizelimit 30965248 /dev/loop1 hdd.img
sudo mkfs.ext2 /dev/loop1

sudo mount /dev/loop1 mnt

sudo mkdir mnt/bin
sudo cp ../userspace/init/init.elf mnt/bin/init
sudo cp ../userspace/hexdump/hexdump.elf mnt/bin/hexdump
sudo cp ../userspace/ushell/ushell.elf mnt/bin/ush

sudo umount mnt
sudo losetup -D
rm -rf mnt
