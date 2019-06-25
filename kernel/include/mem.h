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
    MMR_USER        = BIT(0),
    MMR_RDWR        = BIT(1),
    MMR_READONLY    = BIT(2),
    MMR_SUPV        = BIT(3),
    MMR_EXEC        = BIT(4)
};

struct mmr_struct
{
    void *start;
    void *end;
    size_t size;
    unsigned long flags;

    struct mmr_struct *next_region;
};

struct mm_tables_struct;
struct mm_struct
{
    struct mm_tables_struct *tables;
    struct mmr_struct *kernel_regions;
    struct mmr_struct *regions;
};

struct mm_struct *mk_mmap(const char *description);
int mm_kregion(void *start, void *end);
int mm_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags);

#endif // MEM_H
