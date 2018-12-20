#ifndef KHEAP_H
#define KHEAP_H

#include <util/types.h>
#include <memory/paging.h>

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

#define KHEAP_STATIC_LOCATION   0x600000    // 6MB pre-paging heap start
#define DEFAULT_KHEAP_SIZE      1024*1024*3 // 3MB pre-paging heap size

#define DESC_LENGTH 32                      // Max length of debug description
typedef struct kheap_entry_struct kheap_entry_t;
struct kheap_entry_struct
{
    uint8_t         available;                  // 1 = unallocated, 0 = allocated
    uint32_t        size;                       // actual payload size (without header)
    void *          start;                      // start address of payload memory
    kheap_entry_t * next;                       // next kheap_entry_t
    kheap_entry_t * previous;                   // previous kheap_entry_t
    char            description[DESC_LENGTH];   // description for debugging purposes
} __attribute__((packed));

void *  kmalloc (size_t size, int alignment, char *description);
void    kfree   (void *mem);

int     pheap_valid_addr(unsigned long fault_addr); // called by the page fault handler to verify
                                                    // the validity of the fault_addr

// debug: print all heap contents to the kernel log
void heap_dump(void);

#endif // KHEAP_H
