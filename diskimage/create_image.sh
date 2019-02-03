#!/bin/sh

rm -f hdd.img
mkdir -p mnt

cp hdd.img.template hdd.img

sudo umount /mnt 2> /dev/null
sudo losetup -d /dev/loop20 2> /dev/null
sudo losetup /dev/loop20 hdd.img -P

sudo mount /dev/loop20p1 /mnt

sudo mkdir /mnt/bin
sudo mkdir /mnt/dev
sudo mknod /mnt/dev/tty c 1 0
sudo mknod /mnt/dev/tty1 c 1 1
sudo mknod /mnt/dev/tty2 c 1 2
sudo mknod /mnt/dev/kbd c 2 0
sudo mknod /mnt/dev/hda c 8 0
sudo mknod /mnt/dev/hda1 c 8 1

sudo cp grub.cfg /mnt/boot/grub/grub.cfg
sudo cp ../vmulmix.elf /mnt/vmulmix.elf

sudo cp ../userspace/coreutils/coreutils.elf /mnt/bin/coreutils
sudo ln -s /bin/coreutils /mnt/bin/ls
sudo cp ../userspace/init/init.elf /mnt/bin/init
sudo cp ../userspace/hexdump/hexdump.elf /mnt/bin/hexdump
sudo cp ../userspace/ushell/ushell.elf /mnt/bin/ush

sudo umount /mnt
sudo losetup -d /dev/loop20
#sudo rm -rf mnt
