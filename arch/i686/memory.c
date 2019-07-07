#include <types.h>
#include "mem.h"

#define GB4 0xffffffff

size_t __init memscan(struct mb_struct *mb)
{
    size_t physical_size = 0;

    struct mmape_struct *end =
            (struct mmape_struct*)((char*)mb->mmap + mb->mmap_length);
    for (struct mmape_struct *entry = mb->mmap;
         entry < end;
         entry = (struct mmape_struct*)((char*)entry + entry->entry_size + 4)) {
        if (entry->base_addr <= GB4 && entry->size != 0) {
            if (entry->base_addr + entry->size >= GB4) {
                entry->size = GB4 - entry->base_addr;
            }
        }
        physical_size += entry->size;
    }

    return physical_size;
}
