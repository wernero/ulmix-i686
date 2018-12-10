#!/bin/sh

mv hdd.img hdd.img.bak

rm hdd.img 

bximage -mode=create -hd=30M -q hdd.img
sfdisk hdd.img < sfdisk.layout
sfdisk -l hdd.img

sudo losetup -o 512 --sizelimit 30965248 /dev/loop1 hdd.img
sudo mkfs.ext2 /dev/loop1

sudo mkdir /mnt/hd1p1 ; mount /dev/loop1 /mnt/hd1p1
sudo touch /mnt/hd1p1/this-is-a-disk-image
sudo echo "this is a disk image" > /mnt/hd1p1/readme.txt

sudo umount /mnt/hd1p1/
sudo losetup -l
sudo losetup -D
