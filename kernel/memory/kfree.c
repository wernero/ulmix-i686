#include <heap.h>
#include <sync.h>
#include <debug.h>

extern mutex_t _heap_mtx;

static struct kheape_struct *merge_entries(
        struct kheape_struct *first, struct kheape_struct *second)
{
    ASSERT(first < second);
    ASSERT(first->available && second->available);
    ASSERT(second->previous == first);

    first->next = second->next;
    first->start = first + 1;
    second->next->previous = first;
    first->size = ((unsigned long)second->start + second->size)
            - first->start;

    return first;
}

void kfree(void *ptr)
{
    mutex_lock(&_heap_mtx);

    // do not free NULL pointers or obviously invalid ones
    if (ptr < sizeof(struct kheape_struct))
        return;

    struct kheape_struct *heap_entry =
            ptr - sizeof(struct kheape_struct);

    // check for signs of an invalid pointer
    if (heap_entry->available ||
        heap_entry->size == 0 ||
        heap_entry->start == NULL)
        return;

    // mark the entry as free
    heap_entry->available = 1;
    heap_entry->description = NULL;

    // defragment by merging continuous free blocks
    if (heap_entry->previous->available)
        heap_entry = merge_entries(heap_entry->previous, heap_entry);
    if (heap_entry->next->available)
        merge_entries(heap_entry, heap_entry->next);

    mutex_unlock(&_heap_mtx);
}
