#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "util/types.h"
#include "sched/task.h"

typedef struct
{
    uint16_t size;
    uint32_t addr;
} __attribute__((packed)) idt_descriptor_t;

#define IDT_ENTRIES 256
typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

#define INT_GATE    0x0e
#define INT_TRAP    0x0f
#define INT_PRESENT 0x80
#define INT_SUPV    0x00
#define INT_USER    0x60

#define EOI_PIC1    outb(0x20, 0x20);
#define EOI_PIC2    outb(0x20, 0x20); \
                    outb(0xa0, 0x20);

typedef struct
{
    void (*handler)(void);
    int handler_count;
    uint32_t executions;
    thread_t *unblock;
} interrupt_t;

void setup_idt(void);
int unblock_on_irq(int irq, thread_t *task);
void irq_install_handler(int id, void (*handler)(void));
void irq_install_raw_handler(int id, void (*handler)(void), int flags);

#endif // INTERRUPTS_H
