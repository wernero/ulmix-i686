#ifndef MEM_H
#define MEM_H

#define PAGESIZE    4096
#define HEAP_START  0xc0000000
#define HEAP_END    0xffffffff

#include <heap.h>

/*
 * updates the value stored in the Task State
 * Segment within the GDT that is assigned to
 * the stack pointer whenever a context switch
 * into kernel mode happens.
 *
 * (more info in i386/gdt.h)
 */
void update_tss(unsigned long sp);

/*
 * Memory Regions:
 * struct mmr_struct: memory regions linked list
 */
enum mmr_flags
{
    MMR_USER        = 0x01,
    MMR_RDWR        = 0x02,
    MMR_READONLY    = 0x04,
    MMR_SUPV        = 0x08,
    MMR_EXEC        = 0x10
};

struct mmr_struct
{
    void *start;
    void *end;
    size_t size;
    unsigned long flags;

    struct mmr_struct *next_region;
};

struct mm_struct
{
    void *tables;
    struct mmr_struct *kernel_regions;
    struct mmr_struct *regions;
};

struct mm_struct *mk_mmap(const char *description   );
int mm_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags);

#endif // MEM_H
