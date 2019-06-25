#include <mem.h>
#include <string.h>
#include <debug.h>

#include "paging.h"

extern struct mm_struct *current_mm;

static void map_pages(struct mm_struct *mmap, size_t start_page, size_t pages, void *start_addr, int crflags)
{
    size_t pt_offset = start_page % 512;
    size_t pd_offset = start_page / 512;

    size_t phys_addr = (size_t)start_addr & 0xfffff000;
    while (pages--)
    {
        uint64_t *pt_entry = get_pt_entry(mmap, pd_offset, pt_offset);

        if (*pt_entry & PG_PRESENT)
        {
            // TODO: deallocate old page
        }

        // phys_addr = get_free_page();
        *pt_entry = phys_addr | PG_PRESENT | crflags;

        phys_addr += PAGESIZE;
        pt_offset++;
        if (pt_offset == 512)
        {
            pt_offset = 0;
            pd_offset++;
        }
    }
}

void mm_map(struct mm_struct *mmap, void *start_virt, void *start_phys, size_t n, int crflags)
{
    if ((size_t)start_virt % PAGESIZE != 0
            || (size_t)start_phys % PAGESIZE != 0)
    {
        kprintf("mm_map(): %p -> %p: addr not page aligned\n",
              start_virt, start_phys);
        return;
    }

    if (n < 1)
        return;

    // align size
    size_t page_count = n / PAGESIZE;
    if (n % PAGESIZE != 0)
        page_count++;

    size_t map_start = (size_t)start_virt / PAGESIZE;
    map_pages(mmap, map_start, page_count, start_phys, crflags);
}

uint64_t *get_pt_entry(struct mm_struct *mmap, size_t pd_offset, size_t pt_offset)
{
    uint64_t *ptable = mmap->tables->page_tables[pd_offset];

    if (ptable == NULL) // page table present?
    {
        ptable = kmalloc(PAGEDIR_SIZE, PAGESIZE, "mm_pagetable");
        bzero(ptable, PAGEDIR_SIZE);
        mmap->tables->page_tables[pd_offset] = ptable;

        mmap->tables->page_dirs[0][pd_offset] =
                (size_t)mm_physical(ptable) | PG_PRESENT | PG_USER | PG_RDWR;
    }

    return &(mmap->tables->page_tables[pd_offset][pt_offset]);
}

void *mm_physical(void *addr)
{
    if (current_mm == NULL)
        return addr;

    uint64_t *ptable = current_mm->tables->page_tables[(size_t)addr >> 22];
    uint32_t phys_addr = ptable[((size_t)addr & 0x003ff000) >> 12] & 0xfffff000;
    return (void*)(phys_addr + ((size_t)addr % PAGESIZE));
}
