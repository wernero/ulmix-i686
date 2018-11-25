#ifndef UTIL_H
#define UTIL_H

static inline void nop(void) { __asm__ volatile ("nop"); }
static inline void hlt(void) { __asm__ volatile ("hlt"); }
static inline void sti(void) { __asm__ volatile ("sti"); }
static inline void cli(void) { __asm__ volatile ("cli"); }

#endif
