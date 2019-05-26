#include "mem.h"
#include "multiboot.h"
#include "idt.h"
#include <debug.h>
#include <string.h>

// linker defined:
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
extern char _kernel_end;

// param.c:
extern unsigned long __kernel_start;
extern unsigned long __kernel_end;
extern unsigned long __bss_start;
extern unsigned long __bss_end;
extern unsigned long __ram_size;

void _ksetup(struct mb_struct *mb)
{
    // clear uninitialized data
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    __kernel_start = (unsigned long)&_kernel_beg;
    __kernel_end = (unsigned long)&_bss_start;
    __bss_start = (unsigned long)&_bss_start;
    __bss_end = (unsigned long)&_bss_end;

    setup_gdt();
    setup_idt();

    __ram_size = memscan(mb);

    debug(L_INFO, "lk 24:32\n"
          "ULMIX Operating System\n"
          "kernel at %p (size %S)\n"
          "GDT, IDT ok\n"
          "RAM size: %S\n",
          &_kernel_beg, (&_bss_start - &_kernel_beg),
          __ram_size);

    //setup_timer();
    //setup_paging(ram_available);
}

