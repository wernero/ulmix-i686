#ifndef IDT_H
#define IDT_H

#include <types.h>

#define INT_GATE    0x0e
#define INT_TRAP    0x0f
#define INT_PRESENT 0x80
#define INT_SUPV    0x00
#define INT_USER    0x60

struct idt_desc_struct
{
    uint16_t size;
    uint32_t addr;
} __attribute__((packed));

#define IDT_ENTRIES 256
struct idt_entry_struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type;
    uint16_t offset_high;
} __attribute__((packed));

void set_idt_entry(int id, void (*handler)(void), int flags);
void setup_idt(void);

#endif // IDT_H
