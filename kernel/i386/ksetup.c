#include <ulmix.h>
#include <debug.h>
#include <string.h>

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
extern unsigned long __ram_size;
extern void* __init_brk;

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

    // TODO: load init ramdisk
    __init_brk = __bss_end;

    // initialize memory
    __ram_size = memscan(mb);
    setup_paging();


    //setup_timer();
    //setup_paging(ram_available);
}

