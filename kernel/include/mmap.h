#ifndef MMAP_H
#define MMAP_H

#include <heap.h>
#include <memory.h>

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
    struct mmr_struct *regions;
};

struct mm_struct *mk_mmap(const char *description);
int mm_kregion(void *start, void *end);
int mm_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags);

#endif // MMAP_H
