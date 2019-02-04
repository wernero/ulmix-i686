#!/bin/bash


qemu-system-i386 -kernel vmulmix.elf -drive format=raw,file=diskimage/hdd.img -serial file:serial -netdev tap,id=br0 -device rtl8139,netdev=br0,mac=ab:cd:ef:ab:cd:ef &
cat serial
