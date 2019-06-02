#ifndef PAGING_H
#define PAGING_H

#include <mem.h>
#include <types.h>

#define PAGEDIR_SIZE (4*1024)

#define PG_PRESENT  0x01
#define PG_RDWR     0x02
#define PG_RDONLY   0x00
#define PG_USER     0x04
#define PG_SUPV     0x00

struct mm_ptab_struct
{
    uint32_t *pagedir;
    uint32_t *ptables[1024];
};

extern void *__modules_end;
extern unsigned long __ram_size;
extern void paging_enable(void);

// paging helper functions:

/* mm_physical() translates a virtual address
 * into a physical address */
void *mm_physical(void *addr);

/*
 * get_pt_entry() obtains a pointer to a four-byte page table
 * entry determined by offset in the page directory and offset
 * in the page table.
 * if the page table does not exist, it is created. */
uint32_t *get_pt_entry(struct mm_struct *mmap, size_t pd_offset, size_t pt_offset);

/* mm_map() maps a virtual memory region directly to a contiguous
 * region of physical memory. this can also be used to create
 * identity mappings (when start_virt == start_phys) */
void mm_map(struct mm_struct *mmap, void *start_virt, void *start_phys, size_t n, int crflags);

#endif // PAGING_H
