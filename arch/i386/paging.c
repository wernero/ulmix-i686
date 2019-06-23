#include <mem.h>
#include <debug.h>
#include <string.h>

#include "memory.h"
#include "paging.h"

struct mm_struct *current_mm; // mmap currently in use
static struct mm_struct *mm_kernel;  // mmap with kernel mappings

struct mm_struct *mk_mmap(const char *description)
{
    struct mm_struct *mm = kmalloc(sizeof(struct mm_struct), 1, description);
    mm->tables = kmalloc(sizeof(struct mm_ptab_struct), 1, description);
    mm->tables->pagedir = kmalloc(PAGEDIR_SIZE, PAGESIZE, description);
    bzero(mm->tables->pagedir, PAGEDIR_SIZE);
    bzero(mm->tables->ptables, PAGEDIR_SIZE);
    mm->regions = NULL;
    return mm;
}

void apply_mmap(struct mm_struct *mmap)
{
    void *pagedir = mm_physical(mmap->tables->pagedir);
    current_mm = mmap;
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}

extern char _bss_end;

void setup_paging(void)
{
    setup_pfalloc();

    mm_kernel = mk_mmap("mm_kernel");
   // mm_kregion(mm_kernel, (void*)0x00000000, __modules_end);
   // mm_kregion(mm_kernel, (void*)0xc0000000, (void*)0xffffffff);

    /* actually map the kernel binary and data manually before
     * the activation of paging. otherwise, the CPU can't access
     * the page fault handler and the kernel will blow up -.- */
    mm_map(mm_kernel, 0x0, 0x0, (size_t)&_bss_end, PG_SUPV | PG_RDWR);

    apply_mmap(mm_kernel);
    paging_enable();
}
