#!/bin/bash


qemu-system-i386 -kernel vmulmix.elf -drive format=raw,file=diskimage/hdd.img -serial file:serial -device rtl8139,netdev=net0 -netdev user,id=net0 &
cat serial
