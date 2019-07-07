#include <mmap.h>
#include <errno.h>
#include <debug.h>

static int mm_conflicts(struct mmr_struct *regions, void *start, void *end)
{
    struct mmr_struct *region;
    for (region = regions; region != NULL; region = region->next_region)
    {
        if (start >= region->start && start < region->end)
            return 1;
        if (end > region->start && end <=  region->end)
            return 1;
        if (start <= region->start && end >= region->end)
            return 1;
    }

    return 0;
}

struct mmr_struct *mm_mk_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags)
{
    if (mm_conflicts(mmap->user_regions, start, end))
        return NULL;

    struct mmr_struct *region = kmalloc(sizeof(struct mmr_struct), 1, "mmr_struct");
    region->start = start;
    region->size = (size_t)end - (size_t)start;
    region->end = end;
    region->flags = flags;
    region->next_region = NULL;
    region->prev_region = NULL;

    struct mmr_struct *existing = mmap->user_regions;
    mmap->user_regions = region;
    if (existing != NULL)
    {
        existing->prev_region = region;
        region->next_region = existing;
    }

    kprintf("mm_mk_region(): start=%p, end=%p\n", start, end);

    return region;
}

struct mmr_struct *mm_find_region(struct mm_struct *mmap, void *addr_in_region)
{
    struct mmr_struct *region;
    for (region = mmap->user_regions; region != NULL; region = region->next_region)
    {
        if (addr_in_region >= region->start
            && addr_in_region < region->end)
            return region;
    }
    return NULL;
}

void mm_del_region(struct mm_struct *mmap, struct mmr_struct *region)
{
    region->prev_region->next_region = region->next_region;
    region->next_region->prev_region = region->prev_region;

    // TODO: unmap pagetables
    ASSERT(0);
    (void)mmap;

    kfree(region);
}

int mm_mod_region(struct mm_struct *mmap, struct mmr_struct *region,
                  void *new_start, void *new_end, enum mmr_flags new_flags)
{
    // remove from the linked list
    region->prev_region->next_region = region->next_region;
    region->next_region->prev_region = region->prev_region;

    // check if there are conflicts
    if (mm_conflicts(mmap->user_regions, new_start, new_end))
        return -ECONFLICT;

    // update values
    region->start = new_start;
    region->end = new_end;
    region->size = (unsigned long)new_end - (unsigned long)new_start;
    region->next_region = NULL;
    region->prev_region = NULL;
    region->flags = new_flags;

    // re-add to the linked list
    struct mmr_struct *existing = mmap->user_regions;
    mmap->user_regions = region;
    if (existing != NULL)
    {
        existing->prev_region = region;
        region->next_region = existing;
    }

    // TODO: remap pagetables
    ASSERT(0);

    return SUCCESS;
}
