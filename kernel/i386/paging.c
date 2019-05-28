#include "memory.h"
#include <mem.h>
#include <debug.h>
#include <string.h>

#define PAGEDIR_SIZE (4*1024) // 1024 * 4 bytes
#define mm_physical(a) a

extern void *__init_brk;
extern unsigned long __ram_size;
extern void paging_enable(void);

static struct mm_struct *mm_kernel;

struct mm_struct *mk_mmap(const char *description)
{
    struct mm_struct *mm = kmalloc(sizeof(struct mm_struct), 1, description);
    mm->tables = kmalloc(PAGEDIR_SIZE, PAGESIZE, description);
    bzero(mm->tables, PAGEDIR_SIZE);
    mm->regions = NULL;
    return mm;
}

void apply_mmap(struct mm_struct *mmap)
{
    // get physical
    void *pagedir = mm_physical(mmap->tables);
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}

void setup_paging(void)
{
    setup_pfalloc();

    mm_kernel = mk_mmap("mm_kernel");
    mm_region(mm_kernel, (void*)0x00000000, __init_brk, MMR_EXEC | MMR_SUPV | MMR_RDWR);
    mm_region(mm_kernel, (void*)0xc0000000, (void*)0xffffffff, MMR_SUPV | MMR_RDWR);

  //  apply_pagedir(pagedir_kernel);
  //  paging_enable();
}
