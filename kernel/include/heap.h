#ifndef KHEAP_H
#define KHEAP_H

#include <types.h>

/*
 * One of the most important components of the Ulmix Operating System is the
 * kernel heap. Initially (while paging is disabled), the kernel heap is located
 * at physical memory address KHEAP_STATIC_LOCATION and has a size of STATIC_KHEAP_SIZE.
 *
 * While paging is disabled, the kernel heap will not grow beyond STATIC_KHEAP_SIZE. However,
 * once paging is enabled, the kernel heap will grow until it reaches it's maximum size of 1 GB.
 * kernel heap memory is only allocated if it is actually accessed (via page fault handler).
 *
 * Kheap memory layout:
 *
 *              start addr              size                    address space
 * paging off   KHEAP_STATIC_LOCATION   DEFAULT_KHEAP_SIZE      physcial
 * paging on    3GB                     1GB                     all virtual address spaces
 */

struct kheape_struct
{
    unsigned int            available;      // 1 = unallocated, 0 = allocated
    unsigned long           size;           // actual payload size (without header)
    void *                  start;          // start address of payload memory
    struct kheape_struct *  next;           // next kheap_entry_t
    struct kheape_struct *  previous;       // previous kheap_entry_t
    const char *            description;    // description for debugging purposes
} __attribute__((packed));

void setup_heap(void *start_addr, size_t max_size);
void* kmalloc(size_t size, size_t align, const char *description);
void kfree(void *ptr);

void heap_dump(void);

#endif // KHEAP_H
