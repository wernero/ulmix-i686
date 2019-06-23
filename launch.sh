#!/bin/bash

qemu-system-i386 \
    -kernel vmulmix \
    -serial file:/dev/stdout
