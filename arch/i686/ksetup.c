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

static struct mb_struct __initdata multiboot;

extern void kmain(int argc, char *argv[]);

void __init ksetup(struct mb_struct *mb)
{
    // clear uninitialized data
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    /* multiboot structure has to be saved, otherwise
     * heap or stack will overwrite it sooner or later. */
    multiboot = *mb;
    mb = &multiboot;

    setup_gdt();
    setup_idt();

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

    kmain(0, NULL);
}

