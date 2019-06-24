#include <debug.h>
#include <string.h>
#include <heap.h>

#include "memory.h"
#include "multiboot.h"
#include "idt.h"

// linker defined:
extern char _bss_start;
extern char _bss_end;
extern char _kernel_start;
extern char _kernel_end;

unsigned long __ram_size;

static struct mb_struct multiboot;

extern void setup_vga();

static void iosetup()
{
    setup_vga();
}

void __init ksetup(struct mb_struct *mb)
{
    // clear uninitialized data
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    /* multiboot structure has to go into BSS, otherwise
     * heap or stack will overwrite it sooner or later. */
    multiboot = *mb;
    mb = &multiboot;

    setup_gdt();
    setup_idt();

    kprintf("lk 24:32\n"
          "ULMIX Operating System\n"
          "kernel at %p (size %S)\n"
          "GDT, IDT ok\n",
          &_kernel_start, ((unsigned long)&_bss_start - (unsigned long)&_kernel_start));

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
    //__modules_end = __bss_end;

    __ram_size = memscan(mb);
    setup_paging();

#ifdef _DEBUG_
    heap_dump();
#endif

    iosetup();

    setup_heap((void*)0xc0000000, 0xffffffff - 0xc0000000);
}

