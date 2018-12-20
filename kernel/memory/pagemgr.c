#include "pagemgr.h"
#include "kheap.h"
#include <util/util.h>
#include <log.h>

typedef struct
{
    uint8_t  references;
} page_t;

static page_t *dyn_pages;
static unsigned long pages;

/*
 * setup_pagemgr(): sets up dynamic page allocation for paging
 * this has to be called BEFORE paging is set up
 */
void setup_pagemgr(unsigned long available_memory)
{
    unsigned long dynamic = available_memory - DYNAMIC_START;    // dynamic mem available
    pages = dynamic / PAGESIZE;                             // amount of available pages

    if (dynamic % PAGESIZE != 0)
    {
        klog(KLOG_WARN, "setup_pagemgr(): phys memory size not page aligned");
    }

    klog(KLOG_INFO, "setup_pagemgr(): %d dynamically allocatable pages (%S)",
         pages, dynamic);

    dyn_pages = kmalloc(sizeof(page_t) * pages, 1, "pagemgr array");
    if (dyn_pages == NULL)
    {
        klog(KLOG_FAILURE, "setup_pagemgr(): kmalloc() returned NULL: no memory (!)");
    }

    for (int i = 0; i < pages; i++)
    {
        // initialize page frames
        dyn_pages[i].references = 0;
    }
}

pagetable_entry_t get_free_page(int flags)
{
    for (unsigned long i = 0; i < pages; i++)
    {
        if (dyn_pages[i].references == 0)
        {
            dyn_pages[i].references++;
            unsigned long page_addr = DYNAMIC_START + (i * PAGESIZE);
            return page_addr | PAG_PRESENT | flags;
        }
    }

    klog(KLOG_FAILURE, "get_free_page(): no more pages available");
    return NULL;
}

void release_page(void *page)
{
    unsigned long page_addr = (unsigned long)page;
    if (page_addr < DYNAMIC_START || page_addr % PAGESIZE != 0)
    {
        klog(KLOG_WARN, "release_page(): invalid page address - possible memory leak");
    }

    int array_offset = (page_addr / PAGESIZE) - (DYNAMIC_START / PAGESIZE);
    if (dyn_pages[array_offset].references == 0)
    {
        klog(KLOG_WARN, "release_page(): invalid page address (page not allocated?) - possible mem leak");
    }

    dyn_pages[array_offset].references--;
}
