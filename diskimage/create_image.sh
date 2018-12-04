cp hdd.img hdd.img.bak

rm hdd.img 

bximage -mode=create -hd=30M -sectsize=512 -q hdd.img
sfdisk hdd.img < sfdisk.layout
sfdisk -l hdd.img

losetup -o 32256 --sizelimit 30965248 /dev/loop1 hdd.img
mkfs.ext2 /dev/loop1

mkdir /mnt/hd1p1 ; mount /dev/loop1 /mnt/hd1p1
touch /mnt/hd1p1/this-is-a-disk-image

umount /mnt/hd1p1/
losetup -l
losetup -D
