#ifndef MEM_H
#define MEM_H

/*
 * updates the value stored in the Task State
 * Segment within the GDT that is assigned to
 * the stack pointer whenever a context switch
 * into kernel mode happens.
 *
 * (more info in i386/gdt.h)
 */
void update_tss(unsigned long sp);

#endif // MEM_H
