/*
 * kmain() kernel main function
 * Copyright (C) 2019
 * Written by Alexander Ulmer
 *
 * At this point, the system is already running stable. Architecture
 * dependent code has already set up important system components like
 * the Global Descriptor Table and Interrupt Descriptor Table on x86.
 * A virtual memory environment using paging has been set up as well.
 *
 * As an architecture independent component, kmain() has to call
 * initialization functions on all the loaded drivers and link them
 * against the kernel's symbol table. Furthermore, it has to setup
 * the process scheduler, the final kernel heap at 3GB, and eventually
 * start the first user-process, that will then perform userspace
 * initialization.
 */

#include <debug.h>
#include <heap.h>
#include <sysinfo.h>

extern void iosetup();

void kmain()
{
    kprintf("starting generic system init\n"
            "    CPU:   %s\n", cpu_vendor());

    // let all the drivers register their services
    iosetup();

    setup_heap((void*)0xc0000000, 0xffffffff - 0xc0000000);
}
