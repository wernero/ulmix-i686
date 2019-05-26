#!/bin/bash

qemu-system-i386 \
    -kernel vmulmix.elf \
    -initrd modules/* \
    -serial file:/dev/stdout \
    $1
