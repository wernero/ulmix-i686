#ifndef PAGING_H
#define PAGING_H

/*
 * Before activating paging by calling setup_paging(), the function
 * setup_memory() should be called with the respective multiboot structures
 * as arguments to determine the size of physical memory.
 *
 * setup_memory()       detects available physical RAM
 * setup_paging()       creates a virtual address space for the kernel and enables paging
 * apply_pagedir()      switches virtual address space
 */

#include <util/types.h>

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
#define MB6         (1024*1024*6)
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


uint32_t    setup_memory(void *mmap, uint32_t mmap_len);
void        setup_paging(uint32_t phys_memory);

pagedir_t * mk_user_pagedir(void);
pagedir_t * pagedir_copy(pagedir_t *source);
pagedir_t * get_kernel_pagedir(void);

void        apply_pagedir(void *pagedir);

#endif // PAGING_H
