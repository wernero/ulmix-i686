#include <ulmix.h>
#include <heap.h>
#include <debug.h>

struct kheape_struct *_heap_start = NULL;

void __init setup_heap(void *start_addr, size_t max_size)
{
    struct kheape_struct *first_entry = (struct kheape_struct*)start_addr;
    first_entry->available = 1;
    first_entry->description = NULL;
    first_entry->next = NULL;
    first_entry->previous = NULL;
    first_entry->size = max_size - sizeof(struct kheape_struct);
    first_entry->start = start_addr + sizeof(struct kheape_struct);

    _heap_start = first_entry;
}

void heap_dump(void)
{
    struct kheape_struct *entry;
    for (entry = _heap_start; entry != NULL; entry = entry->next)
    {
        debug(L_DEBUG, "avlb=%s, start=%p, size=%S, \"%s\"\n",
              entry->available ? "yes" : "no ",
              entry->start,
              entry->size,
              entry->description);
    }
}