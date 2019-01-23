#!/bin/bash


qemu-system-i386 -kernel vmulmix.elf -drive format=raw,file=diskimage/hdd.img -serial file:serial &
cat serial
