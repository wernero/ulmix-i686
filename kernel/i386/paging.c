#include "memory.h"
#include <mem.h>
#include <debug.h>
#include <string.h>

#define PAGEDIR_SIZE (4*1024) // 1024 * 4 bytes
#define mm_physical(a) a

extern void *__modules_end;
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

static void mm_map(struct mm_struct *mmap, void *start_virt, void *start_phys, size_t n)
{
    if ((size_t)start_virt % PAGESIZE != 0
            || (size_t)start_phys % PAGESIZE != 0)
    {
        debug(L_ERROR, "mm_idmap(): %p -> %p: addr not page aligned\n",
              start_virt, start_phys);
        return;
    }

    if (n < 1)
        return;

    // align size
    size_t page_count = n / PAGESIZE;
    if (n % PAGESIZE != 0)
        page_count++;


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
    mm_region(mm_kernel, (void*)0x00000000, __modules_end, MMR_EXEC | MMR_SUPV | MMR_RDWR);
    mm_region(mm_kernel, (void*)0xc0000000, (void*)0xffffffff, MMR_SUPV | MMR_RDWR);

    /* actually map the kernel binary and data manually before
     * the activation of paging. otherwise, the CPU can't access
     * the page fault handler and the kernel will blow up -.- */
    // mm_map(mm_kernel, 0x0, 0x0, (size_t)__init_brk);

    // apply_pagedir(pagedir_kernel);
    // paging_enable();

    // setup_heap(HEAP_START, HEAP_END - HEAP_START);
}
