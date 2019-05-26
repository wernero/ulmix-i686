#!/bin/bash

qemu-system-i386 \
    -kernel vmulmix.elf \
    -serial file:/dev/stdout
