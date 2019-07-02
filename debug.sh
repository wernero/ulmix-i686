#!/bin/bash

qemu-system-i386 -kernel vmulmix -serial stdio -drive format=raw,file=diskimage/hdd.img -S -s
