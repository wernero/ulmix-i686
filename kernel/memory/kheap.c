#include "kheap.h"
#include "util/util.h"
#include "util/string.h"
#include "log.h"

extern int paging_enabled;
extern char _kernel_end;

static int heap_enabled = 0;
static int pheap_enabled = 0;
static uint32_t kheap_size;
static kheap_entry_t *heap;

static void             setup_heap      (uint32_t heap_start);
static void             setup_pheap     (void);
static void *           merge_blocks    (kheap_entry_t *entry1, kheap_entry_t *entry2, char *description);
static kheap_entry_t *  find_free_block (size_t *alignment_offset, size_t size, int alignment);
static kheap_entry_t *  mkblock         (kheap_entry_t *previous, kheap_entry_t *next,
                                            int available, size_t size, void *start, void *description);

void heap_dump(void); // dump kheap linked list

/*
 * find_free_block(): finds a free block in the heap linked list that meeets
 *  the requirements of size and alignment of the start address.
 * 'alignment_offset' will be manipulated according to the offset between start
 * address of the new block and the required aligned start address of the actual payload
 * returns: a pointer to the descriptor of the block that was found.
 */
static kheap_entry_t *find_free_block(size_t *alignment_offset, size_t size, int alignment)
{
    kheap_entry_t *entry;
    for(entry = heap; entry != NULL; entry = (kheap_entry_t*)entry->next)
    {
        if (entry->available && entry->size >= size)
        {
            for (size_t i = 0; i < entry->size; i++)
            {
                if ((uint32_t)((char*)entry->start + i) % alignment == 0 &&
                        (entry->size - i) >= size)
                {
                    *alignment_offset = i;
                    return entry;
                }
            }
        }
    }

    return NULL;
}


/**
 * pheap_valid_addr() verifies that 'fault_addr' is a allocated heap region.
 */
int pheap_valid_addr(unsigned long fault_addr)
{
    if (fault_addr < GB3)
        return 0;

    if (!pheap_enabled)
        return 1;

    kheap_entry_t *entry;
    for(entry = heap; entry != NULL; entry = (kheap_entry_t*)entry->next)
    {
        if (entry->available
                && fault_addr >= (unsigned long)entry->start
                && fault_addr < ((unsigned long)entry->start + entry->size))
            return 1;
    }

    return 0;
}

/*
 * kmalloc(): allocate a block of memory on the kernel heap
 * the block of memory can be aligned to start at an address divisable by 'alignment'.
 * the 'description' string will show up on debug output.
 * returns: a pointer to the allocated block of memory, or NULL on error
 */
void *kmalloc(size_t size, int alignment, char *description)
{
    if (alignment == 0) alignment = 1;
    if (!pheap_enabled)
    {
        if (!heap_enabled)  setup_heap((uint32_t)KHEAP_STATIC_LOCATION);
        if (!pheap_enabled && paging_enabled) setup_pheap();
    }

    klog(KLOG_DEBUG, "kmalloc(): size=%S align=%S purpose=%s", size, alignment, description);

    size_t alignment_offset;
    kheap_entry_t *block;
    block = find_free_block(&alignment_offset, size, alignment);
    if (block == NULL)
    {
        klog(KLOG_PANIC, "kmalloc(): no more memory");
        return NULL;
    }

    kheap_entry_t *new_block;
    size_t excess_size = block->size - alignment_offset - size;
    if (alignment_offset == 0)
    {
        // if the alignment is already correct on that block, use it right away
        strcpy(block->description, description);
        block->description[DESC_LENGTH - 1] = 0;
        block->available = 0;
        block->size = size;
        new_block = block;
    }
    else if (alignment_offset <= sizeof(kheap_entry_t))
    {
        // can't fit another block, so make the last one bigger
        // and move the descriptor

        strcpy(block->description, description);
        block->description[DESC_LENGTH - 1] = 0;
        block->available = 0;
        block->size = size;
        block->start = (char*)block->start + alignment_offset;

        ((kheap_entry_t*)block->previous)->size += alignment_offset;
        new_block = memmove((char*)block + alignment_offset, block, sizeof(kheap_entry_t));
    }
    else
    {
        // another block will fit in the alignment gap, so create it

        new_block = mkblock(block, (kheap_entry_t*)block->next,
                            0, size, block->start + alignment_offset, description);

        block->description[0] = 0;
        block->available = 1;
        block->size = alignment_offset - sizeof(kheap_entry_t);
        block->next = new_block;
    }

    // since we're allocating, there has to come a free block after our's
    // to describe the excess size. Exception: excess size <= sizeof(kheap_entry_t)
    if (excess_size > sizeof(kheap_entry_t))
    {
        new_block->next = mkblock(new_block,
                (kheap_entry_t*)new_block->next,
                1,
                excess_size - sizeof(kheap_entry_t),
                new_block->start + new_block->size + sizeof(kheap_entry_t),
                NULL);
    }
    else
    {
        new_block->size += excess_size;
    }

    klog(KLOG_DEBUG, "kmalloc(): returned %x", new_block->start);
    return new_block->start;
}

/*
 * kfree(): releases memory allocated by kmalloc().
 * it also tries to merge with neighbouring unallocated blocks if possible.
 */
void kfree(void *mem)
{
    kheap_entry_t *entry = (kheap_entry_t*)(mem - sizeof(kheap_entry_t));

    if (entry->start != mem || entry->available != 0)
    {
        klog(KLOG_WARN, "kheap: kfree(): possible memory leak due to corrupt pointer");
        return;
    }

    int merged = 0;
    entry->available = 1;
    kheap_entry_t *merged_entry;
    if (((kheap_entry_t*)entry->previous)->available)
    {
        merged_entry = merge_blocks(entry,
                    (kheap_entry_t*)entry->previous,
                    ((kheap_entry_t*)entry->previous)->description);

        if (merged_entry != NULL)
        {
            merged = 1;
        }
    }

    if (((kheap_entry_t*)entry->next)->available)
    {
        merged_entry = merge_blocks(entry,
                    (kheap_entry_t*)entry->next,
                    ((kheap_entry_t*)entry->next)->description);

        if (merged_entry != NULL)
        {
            merged = 1;
        }
    }

    if (!merged)
    {
        entry->available = 1;
        entry->description[0] = 0;
    }
}


/*
 * setup_heap(): will determine the location of the heap and setup an initial block
 * 'heap_start' specifies the location where the heap will begin (grows towards high addr)
 */
static void setup_heap(uint32_t heap_start)
{
    kheap_size = DEFAULT_KHEAP_SIZE;
    heap = (kheap_entry_t*)heap_start;

    heap->available = 1;
    heap->description[DESC_LENGTH - 1] = 0;
    heap->next = NULL;
    heap->previous = NULL;
    heap->size = kheap_size - sizeof(kheap_entry_t);
    heap->start = (void*)((char*)heap + sizeof(kheap_entry_t));

    heap_enabled = 1;
    klog(KLOG_INFO, "setup_heap(): starting at 0x%x of size %S", (uint32_t)heap, kheap_size);
}

/*
 * setup_pheap(): sets up the paged heap that is located at 3-4GB
 */
static void setup_pheap()
{
    // TODO: free up the space blocked by the static heap
    // so the stack can grow higher

    kheap_size = GB1;
    heap = (kheap_entry_t*)GB3;

    heap->available = 1;
    heap->description[DESC_LENGTH - 1] = 0;
    heap->next = NULL;
    heap->previous = NULL;
    heap->size = kheap_size - sizeof(kheap_entry_t);
    heap->start = (void*)((char*)heap + sizeof(kheap_entry_t));

    pheap_enabled = 1;
    klog(KLOG_INFO, "setup_pheap(): starting at 0x%x of size %S", (uint32_t)heap, kheap_size);
}


/*
 * kheap_dump: will print a list of memory blocks to debug output
 */
void heap_dump(void)
{
    int i = 0;
    kheap_entry_t *entry;
    klog(KLOG_DEBUG, "kheap: heap_dump(): %s HEAP CONTENTS", pheap_enabled ? "DYNAMIC PAGED" : "PRE-PAGING");
    for(entry = heap; entry != NULL; entry = (kheap_entry_t*)entry->next)
    {
        klog(KLOG_DEBUG, "kheap: heap_dump(): #%d: avail=%d, start=0x%x, size=%S (%s)",
             i++,
             entry->available,
             (uint32_t)entry->start,
             entry->size,
             entry->description);
    }
}


/*
 * mkblock(): will create a new block with the specified parameters
 * parameter 'start' will specify the payload start address, so the
 * kheap_entry_t descriptor will be placed at 'start' - sizeof(kheap_entry_t).
 * returns: a pointer to the descriptor of the newly created block
 */
static kheap_entry_t *mkblock(kheap_entry_t *previous, kheap_entry_t *next,
                              int available, size_t size, void *start, void *description)
{
    kheap_entry_t *block = (kheap_entry_t*)((char*)start - sizeof(kheap_entry_t));

    if (description == NULL)
    {
        block->description[0] = 0;
    }
    else
    {
        strcpy(block->description, description);
        block->description[DESC_LENGTH - 1] = 0;
    }

    block->available = available;
    block->next = next;
    block->size = size;
    block->start = start;
    block->previous = previous;
    return block;
}


/*
 * merge_blocks(): merges two blocks of memory that are next to each other.
 * the two blocks of memory have to be unallocated.
 * returns: pointer to the kheap_entry_t descriptor of the new block.
 */
static void *merge_blocks(kheap_entry_t *entry1, kheap_entry_t *entry2, char *description)
{
    if (!entry1->available || !entry2->available)
    {
        klog(KLOG_FAILURE, "kheap: merge_blocks(): can only merge unallocated pages");
        return NULL;
    }

    if (entry1 > entry2)
    {
        kheap_entry_t *temp = entry2;
        entry2 = entry1;
        entry1 = temp;
    }

    if (entry2->previous != entry1)
    {
        klog(KLOG_FAILURE, "kheap: merge_blocks(): can only merge contiguous blocks");
        return NULL;
    }

    strcpy(entry1->description, description);
    entry1->description[DESC_LENGTH - 1] = 0;
    entry1->next = entry2->next;
    entry1->size += entry2->size + sizeof(kheap_entry_t);

    return entry1;
}
