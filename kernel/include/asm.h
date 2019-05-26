#ifndef ASM_H
#define ASM_H

#include <types.h>

// x86 Assembler:

inline void hlt(void) { __asm__ volatile ("hlt"); }
inline void sti(void) { __asm__ volatile ("sti"); }
inline void cli(void) { __asm__ volatile ("cli"); }

inline uint8_t inb(uint16_t port)
{
    uint8_t ret_val;
    __asm__ volatile ("inb %1, %0" : "=a"(ret_val) : "Nd"(port));
    return ret_val;
}

inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}


#endif // ASM_H
