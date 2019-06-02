#include <mem.h>
#include <string.h>

#include "paging.h"

extern struct mm_struct *current_mm;

uint32_t *get_pt_entry(struct mm_struct *mmap, size_t pd_offset, size_t pt_offset)
{
    uint32_t *ptable = mmap->tables->ptables[pd_offset];

    if (ptable == NULL) // page table present?
    {
        ptable = kmalloc(PAGEDIR_SIZE, PAGESIZE, "mm_pagetable");
        bzero(ptable, PAGEDIR_SIZE);
        mmap->tables->ptables[pd_offset] = ptable;

        mmap->tables->pagedir[pd_offset] =
                (size_t)mm_physical(ptable) | PG_PRESENT | PG_USER | PG_RDWR;
    }

    return &(mmap->tables->ptables[pd_offset][pt_offset]);
}

void *mm_physical(void *addr)
{
    if (current_mm == NULL)
        return addr;

    uint32_t *ptable = current_mm->tables->ptables[(size_t)addr >> 22];
    uint32_t phys_addr = ptable[((size_t)addr & 0x003ff000) >> 12] & 0xfffff000;
    return (void*)(phys_addr + ((size_t)addr % PAGESIZE));
}
