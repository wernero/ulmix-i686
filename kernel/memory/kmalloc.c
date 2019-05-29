#include <heap.h>
#include <debug.h>
#include <mem.h>

extern struct kheape_struct *_heap_start;

static struct kheape_struct *
link_new_entry(void *addr, struct kheape_struct *prev)
{
    struct kheape_struct *new_entry = (struct kheape_struct*)addr;

    new_entry->previous = prev;
    new_entry->next = prev->next;
    if (prev->next)
        prev->next->previous = new_entry;
    prev->next = new_entry;

    return new_entry;
}

static void*
alloc_entry(struct kheape_struct *entry, size_t start_offset,
            size_t size, const char *description)
{
    struct kheape_struct *new_entry;
    size_t prev_size = entry->size;

    if (start_offset >= 4 * sizeof(struct kheape_struct))
    {
        new_entry = link_new_entry(entry->start + start_offset -
                                   sizeof(struct kheape_struct), entry);
        entry->size = (void*)new_entry - entry->start;
    }
    else
    {
        new_entry = entry;
    }

    new_entry->available = 0;
    new_entry->description = description;
    new_entry->start = entry->start + start_offset;

    if ((prev_size - size) > (4 * sizeof(struct kheape_struct)))
    {
        struct kheape_struct *new_next =
                link_new_entry(new_entry->start + size, new_entry);

        new_next->start = (void*)new_next + sizeof(struct kheape_struct);
        new_next->available = 1;
        new_next->description = NULL;
        new_next->size = prev_size - start_offset
                - size - sizeof(struct kheape_struct);

        new_entry->size = size;
    }
    else
    {
        new_entry->size = prev_size;
    }

    return new_entry->start;
}

static void *
next_aligned_addr(void *addr, size_t align)
{
    size_t remainder = (size_t)addr % align;

    if (remainder == 0)
        return addr;
    return addr + (align - remainder);
}

void *
kmalloc(size_t size, size_t align, const char *description)
{
    struct kheape_struct *entry;
    size_t start_offset, min_size;

    for (entry = _heap_start; entry != NULL; entry = entry->next)
    {
        start_offset = (size_t)next_aligned_addr(entry->start, align)
                - (size_t)entry->start;
        min_size = size + start_offset;

        if (entry->available && entry->size >= min_size)
            return alloc_entry(entry, start_offset, size, description);
    }

    debug(L_ERROR, "kmalloc(): error: no more memory (%s)\n", description);
    return NULL;
}
