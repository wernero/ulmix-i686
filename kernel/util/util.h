#ifndef UTIL_H
#define UTIL_H

#include "util/types.h"

static inline void nop(void) { __asm__ volatile ("nop"); }
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

void *memset(void *mem, uint8_t value, size_t len);
void *memcpy(void *dest, void *src, size_t n);
void *bzero(void *mem, size_t len);

#endif
