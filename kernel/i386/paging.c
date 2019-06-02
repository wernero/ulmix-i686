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

static void map_pages(struct mm_struct *mmap, size_t start_page, size_t pages, void *start_addr, int crflags)
{
    size_t pt_offset = start_page % 1024;
    size_t pd_offset = start_page / 1024;

    size_t phys_addr = (size_t)start_addr & 0xfffff000;
    while (pages--)
    {
        uint32_t *pt_entry = get_pt_entry(mmap, pd_offset, pt_offset);

        if (*pt_entry & PG_PRESENT)
        {
            // TODO: deallocate old page
        }

        // phys_addr = get_free_page();
        *pt_entry = phys_addr | PG_PRESENT | crflags;

        phys_addr += PAGESIZE;
        pt_offset++;
        if (pt_offset == 1024)
        {
            pt_offset = 0;
            pd_offset++;
        }
    }
}

static void mm_map(struct mm_struct *mmap, void *start_virt, void *start_phys, size_t n, int crflags)
{
    if ((size_t)start_virt % PAGESIZE != 0
            || (size_t)start_phys % PAGESIZE != 0)
    {
        debug(L_ERROR, "mm_map(): %p -> %p: addr not page aligned\n",
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

void apply_mmap(struct mm_struct *mmap)
{
    void *pagedir = mm_physical(mmap->tables->pagedir);
    current_mm = mmap;
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}

void setup_paging(void)
{
    setup_pfalloc();

    mm_kernel = mk_mmap("mm_kernel");
   // mm_kregion(mm_kernel, (void*)0x00000000, __modules_end);
   // mm_kregion(mm_kernel, (void*)0xc0000000, (void*)0xffffffff);

    /* actually map the kernel binary and data manually before
     * the activation of paging. otherwise, the CPU can't access
     * the page fault handler and the kernel will blow up -.- */
    mm_map(mm_kernel, 0x0, 0x0, (size_t)__modules_end, PG_SUPV | PG_RDWR);

    apply_mmap(mm_kernel);
    paging_enable();

    // setup_heap(HEAP_START, HEAP_END - HEAP_START);
}
