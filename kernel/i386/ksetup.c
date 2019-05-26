#include "multiboot.h"
#include <string.h>

// linker defined:
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
extern char _kernel_end;

void _ksetup(struct mb_struct *mb)
{
    // clear uninitialized data
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    //setup_gdt();

    //kprintf(L_DEBUG, "lk 24:32\n"
      //      "ULMIX Operating System\n"
      //      "kernel @ %p - (size %S)\n");

    //setup_idt();
    //setup_timer();

    // TODO: refactor
    //uint32_t ram_available = setup_memory(mb->mmap, mb->mmap_length);
    //setup_paging(ram_available);
}

