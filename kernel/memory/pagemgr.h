#ifndef PAGEMGR_H
#define PAGEMGR_H


/*
 * pagemgr.h: the page manager keeps track of all the page frames
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
 */

#include "memory/paging.h"
#include "util/types.h"

#define DYNAMIC_START   MB16    // start dynamic allocation of pages at phys addr 16MB

void                setup_pagemgr(unsigned long available_memory);
pagetable_entry_t   get_free_page(int flags);
void                release_page(void *page);


#endif // PAGEMGR_H
