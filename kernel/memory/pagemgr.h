#ifndef PAGEMGR_H
#define PAGEMGR_H

#include "memory/paging.h"
#include "util/types.h"

void setup_pagemgr(uint32_t available_memory);
pagetable_entry_t get_free_page(int flags);
void release_page(pagetable_entry_t page);


#endif // PAGEMGR_H
