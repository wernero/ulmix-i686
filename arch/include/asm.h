#ifndef ASM_H
#define ASM_H

#include <types.h>

// x86 Assembler:

static inline void hlt(void) { __asm__ volatile ("hlt"); }
static inline void sti(void) { __asm__ volatile ("sti"); }
static inline void cli(void) { __asm__ volatile ("cli"); }

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret_val;
    __asm__ volatile ("inb %1, %0" : "=a"(ret_val) : "Nd"(port));
    return ret_val;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint32_t get_eflags(void)
{
    uint32_t eflags;
    __asm__ volatile ("pushf; pop %0" : "=r" (eflags));
    return eflags;
}

static inline uint32_t get_esp(void)
{
    uint32_t esp;
    __asm__ volatile ("mov %%esp, %0" : "=r" (esp));
    return esp;
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret_val;
    __asm__ volatile ("inw %1, %0" : "=a"(ret_val) : "Nd"(port));
    return ret_val;
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret_val;
    __asm__ volatile ("inl %1, %0" : "=a"(ret_val) : "Nd"(port));
    return ret_val;
}

static inline void repinsb(uint16_t port, uint8_t* buf, uint32_t count)
{
    uint32_t temp = (uint32_t)buf;
    __asm__ volatile ("rep insb" : "+D"(temp), "+c"(count) : "Nd"(port) : "memory");
}

static inline void repinsw(uint16_t port, uint16_t* buf, uint32_t count)
{
    uint32_t temp = (uint32_t)buf;
    __asm__ volatile ("rep insw" : "+D"(temp), "+c"(count)  : "Nd"(port) : "memory");
}

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %0, %1" :: "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t lng)
{
    __asm__ volatile ("outl %0, %1" :: "a"(lng), "Nd"(port));
}

#endif // ASM_H
