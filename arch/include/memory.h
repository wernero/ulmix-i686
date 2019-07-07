#ifndef MEMORY_H
#define MEMORY_H

#define PAGESIZE    4096
#define HEAP_START  0xc0000000
#define HEAP_END    0xffffffff

/*
 * updates the value stored in the Task State
 * Segment within the GDT that is assigned to
 * the stack pointer whenever a context switch
 * into kernel mode happens.
 *
 * (more info in arch/i686/gdt.h)
 */
void update_tss(unsigned long sp);

#endif // MEMORY_H
