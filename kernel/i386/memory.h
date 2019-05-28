#ifndef MEMORY_H
#define MEMORY_H

#include "multiboot.h"
#include <types.h>

/*
 * setup_gdt():  setup Global Descriptor Table
 *
 * (more info in gdt.h)
 */
void setup_gdt(void);

size_t memscan(struct mb_struct *mb);
void setup_paging(void);

void setup_pfalloc(void);
size_t get_free_page(void);
size_t alloc_page(void *phys_addr);
void release_page(size_t id);

#endif // MEMORY_H
