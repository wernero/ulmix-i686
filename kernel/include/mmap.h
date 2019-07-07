#ifndef MMAP_H
#define MMAP_H

#include <heap.h>
#include <memory.h>

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
    unsigned long size;
    unsigned long flags;

    struct mmr_struct *prev_region;
    struct mmr_struct *next_region;
};

struct mm_tables_struct;
struct mm_struct
{
    struct mm_tables_struct *tables;
    struct mmr_struct *user_regions;
};

// allocate and delete virtual address space
struct mm_struct *mk_mmap(const char *description);
void del_mmap(struct mm_struct *mmap);

// USER memory region control
struct mmr_struct *mm_find_region(struct mm_struct *mmap, void *addr_in_region);
struct mmr_struct *mm_mk_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags);
void mm_del_region(struct mm_struct *mmap, struct mmr_struct *region);
int mm_mod_region(struct mm_struct *mmap, struct mmr_struct *region,
                  void *new_start, void *new_end, enum mmr_flags new_flags);

#endif // MMAP_H
