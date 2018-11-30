#ifndef KHEAP_H
#define KHEAP_H

#include "util/types.h"
#include "memory/paging.h"

#define DEFAULT_KHEAP_SIZE 1024*1024*3

#define DESC_LENGTH 21
typedef struct
{
    uint8_t     available;
    uint32_t    size;
    void        *start;
    void        *next;
    void        *previous;
    char        description[DESC_LENGTH];
} __attribute__((packed)) kheap_entry_t;

void *kmalloc(size_t size, int alignment, char *description);
void kfree(void *mem);

#endif // KHEAP_H
