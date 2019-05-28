#include <ulmix.h>
#include <heap.h>
#include <debug.h>

extern void *__init_brk;
static int heap_enabled = 0;

static void* __init kmalloc_init(size_t size, size_t align, const char *description)
{
    // place a heap struct
    struct kheape_struct *entry = __init_brk;
    entry->available = 0;
    entry->description = description;
    entry->size = size;
    entry->start = (void*)entry + sizeof(struct kheape_struct);

    // ensure alignment:
    while ((unsigned long)entry->start % align != 0)
        entry->start++;

    // not used with init heap:
    entry->previous = NULL;
    entry->next = NULL;

    // adjust heap break
    __init_brk = entry->start + entry->size;
    debug(L_DEBUG, "malloc() brk: start=%p, size=%S, brk=%p\n",
          entry->start, entry->size, __init_brk);
    return entry->start;
}

void* kmalloc(size_t size, size_t align, const char *description)
{
    if (!heap_enabled)
        return kmalloc_init(size, align, description);

    // kmalloc()

    return NULL;
}

void kfree(void *ptr)
{

}
