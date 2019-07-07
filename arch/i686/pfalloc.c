#include "mem.h"
#include <memory.h>
#include <heap.h>
#include <types.h>

/*
 * pfalloc: the page manager keeps track of all the page frames
 * in physical memory. they can be dynamically allocated and deallocated
 * by using the functions get_free_page() and release_page() respectively.
 *
 * setup_pagemgr() has to be called before paging is enabled, because
 * paging heavily depends on the pagemgr. setup_paging() does that for you.
 *
 * get_free_page() is usually called by the page fault handler when some
 * code tries to access a virtual memory location that has not been allocated yet.
 * the page fault handler then assigns that virtual memory location the page that
 * has been returned by get_free_page()
 *
 * if a process dies, the pages allocated by that process should eventually be
 * released by the function release_page().
 *
 * TODO: move to arch-generic
 */

struct pf_struct
{
    uint8_t ref_count;
    // void *phys_addr;
};

static struct pf_struct *pframes;
extern unsigned long __ram_size;

void setup_pfalloc(void)
{
    size_t page_count = __ram_size / PAGESIZE;
    pframes = kmalloc(page_count * sizeof(struct pf_struct), 1, "pf_struct[]");
}

size_t get_free_page(void)
{
    // TODO: update algorithm
    size_t index = 0;
    while ((pframes[index].ref_count) != 0)
        ++index;

    pframes[index].ref_count++;
    return index * PAGESIZE;
}

size_t alloc_page(void *phys_addr)
{
    if ((size_t)phys_addr % PAGESIZE != 0)
        return NULL;

    size_t index = (size_t)phys_addr / PAGESIZE;
    pframes[index].ref_count++;
    return index * PAGESIZE;
}

void release_page(size_t id)
{
    if (pframes[id].ref_count)
        pframes[id].ref_count--;
}
