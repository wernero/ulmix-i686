#include <mem.h>
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

int mm_kregion(void *start, void *end)
{
    return 0;
}

int mm_region(struct mm_struct *mmap, void *start, void *end, enum mmr_flags flags)
{
    if (mm_conflicts(mmap->regions, start, end))
        return -1;

    struct mmr_struct *region = kmalloc(sizeof(struct mmr_struct), 1, "mmr_struct");
    region->start = start;
    region->size = (size_t)end - (size_t)start;
    region->end = end;
    region->flags = flags;
    region->next_region = NULL;

    struct mmr_struct *existing = mmap->regions;
    mmap->regions = region;
    if (existing != NULL)
        region->next_region = existing;

    debug(L_DEBUG, "region start=%p, end=%p\n", start, end);

    return 0;
}
