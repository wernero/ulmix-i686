#include "pagemgr.h"
#include "kheap.h"
#include "util/util.h"
#include "log.h"

typedef struct
{
    uint32_t phys_address;
    uint8_t  references;
} page_t;

static page_t *dyn_pages;
static uint32_t pages;

/*
 * setup_pagemgr(): sets up dynamic page allocation for paging
 * this has to be called BEFORE paging is set up
 */
void setup_pagemgr(uint32_t available_memory)
{
    uint32_t reserved = 1024*1024*16;               // first 16 MB are reserved
                                                    // by the kernel
    uint32_t dynamic = available_memory - reserved; // dynamic mem available
    pages = dynamic / PAGESIZE;            // amount of available pages

    if (dynamic % PAGESIZE != 0)
    {
        pages++;
        klog(KLOG_WARN, "setup_pagemgr(): phys memory size not page aligned");
    }

    klog(KLOG_DEBUG, "setup_pagemgr(): %d dynamically allocatable pages (%S)",
         pages, dynamic);

    dyn_pages = kmalloc(sizeof(page_t) * pages, 1, "pagemgr array");
    if (dyn_pages == NULL)
    {
        klog(KLOG_FAILURE, "setup_pagemgr(): kmalloc() returned NULL: no memory (!)");
    }
}

pagetable_entry_t get_free_page(int flags)
{
    for (uint32_t i = 0; i < pages; i++)
    {
        if (dyn_pages[i].references == 0)
        {
            dyn_pages[i].references++;
            return (dyn_pages[i].phys_address) | PAG_PRESENT | flags;
        }
    }

    klog(KLOG_FAILURE, "get_free_page(): no more pages available");
    return NULL;
}

void release_page(pagetable_entry_t page)
{
    for (uint32_t i = 0; i < pages; i++)
    {
        if ((page >> 16) == dyn_pages[i].phys_address)
        {
            if (dyn_pages[i].references != 0)
            {
                dyn_pages[i].references--;
                return;
            }
        }
    }

    klog(KLOG_WARN, "release_page(): invalid page address - possible memory leak");
}
