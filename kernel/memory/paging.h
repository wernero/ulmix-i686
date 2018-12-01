#ifndef PAGING_H
#define PAGING_H

#include "util/types.h"

#define PAG_RDWR    0x02
#define PAG_RDONLY  0x00
#define PAG_PRESENT 0x01
#define PAG_SUPV    0x00
#define PAG_USER    0x04

#define PAGESIZE        4096
#define PGTABLE_SIZE    4096
#define PGDIR_SIZE      4096

#define MB1         (1024*1024)
#define MB4         (1024*4096)
#define MB16        (4096*1024*4)
#define GB1         (1024*1024*1024)
#define GB3         0xc0000000
#define GB4         0xffffffff

typedef uint32_t pagedir_entry_t;
typedef uint32_t pagetable_entry_t;
typedef struct
{
    pagedir_entry_t pagetables[1024];
} __attribute__((packed)) pagedir_t;
typedef struct
{
    pagetable_entry_t pages[1024];
} __attribute__((packed)) pagetable_t;


uint32_t setup_memory(void *mmap, uint32_t mmap_len);
void     setup_paging(uint32_t phys_memory);
void     apply_pagedir(void *pagedir);

#endif // PAGING_H
