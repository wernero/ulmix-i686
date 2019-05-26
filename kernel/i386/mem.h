#ifndef MEM_H
#define MEM_H

#include "multiboot.h"

/*
 * setup_gdt():  setup Global Descriptor Table
 *
 * (more info in gdt.h)
 */
void setup_gdt(void);


/*
 * multiboot memory scan:
 */
size_t memscan(struct mb_struct *mb);

#endif // MEM_H
