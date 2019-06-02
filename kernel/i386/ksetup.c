#include <ulmix.h>
#include <debug.h>
#include <string.h>
#include <heap.h>

#include "memory.h"
#include "multiboot.h"
#include "idt.h"

// linker defined:
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
extern char _kernel_end;

// param.c:
extern void* __kernel_start;
extern void* __kernel_end;
extern void* __bss_start;
extern void* __bss_end;
extern void* __modules_end;
extern unsigned long __ram_size;

void __init _ksetup(struct mb_struct *mb)
{
    // clear uninitialized data
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    __kernel_start = &_kernel_beg;
    __kernel_end = &_bss_start;
    __bss_start = &_bss_start;
    __bss_end = &_bss_end;

    setup_gdt();
    setup_idt();

    debug(L_INFO, "lk 24:32\n"
          "ULMIX Operating System\n"
          "kernel at %p (size %S)\n"
          "GDT, IDT ok\n",
          __kernel_start, (__bss_start - __kernel_start));

    /* because the eventual kernel heap at 3GB - 4GB
     * is not accessible in a pre-paging environment,
     * an early heap has to be setup. In low memory,
     * the region from 0x00000 - 0x80000 is used to
     * accomodate the kernel's stack and heap.
     *
     * Early Kernel Stack: growing down from 0x80000
     * Early Kernel Heap: growing up from 0x00001
     *
     * (0x00001 because null pointer is not allowed)
     *
     * Caution: Realmode IVT at 0x000-0x400 and BIOS
     * data area at 0x400 - 0x500 may cause trouble. */
    setup_heap((void*)0x00001, 0x80000);

    // TODO: load init ramdisk
    __modules_end = __kernel_end;

    // initialize memory
    __ram_size = memscan(mb);
    setup_paging();

    // setup_timer();
    // setup scheduler

    heap_dump();
}

