#include <asm.h>

#include "idt.h"

static struct idt_entry_struct idt[IDT_ENTRIES];

static void __init pic_init(void)
{
    outb(0x20, 0x11);   // init PIC1
    outb(0xA0, 0x11);   // init PIC2

    outb(0x21, 0x20);   // start interrupts at 0x20 on PIC1
    outb(0xA1, 0x28);   // start interrupts at 0x28 on PIC2

    outb(0x21, 0x04);   // PIC1 -> master
    outb(0xA1, 0x02);   // PIC2 -> slave

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);   // unmask all interrupts PIC1
    outb(0xA1, 0x00);   // unmask all interrupts PIC2
}

void __init set_idt_entry(int id, void (*handler)(void), int flags)
{
    idt[id].offset_low   = (uint32_t)handler & 0x0000ffff;
    idt[id].offset_high  = ((uint32_t)handler & 0xffff0000) >> 16;
    idt[id].zero         = 0x00;
    idt[id].type         = flags;
    idt[id].selector     = 0x0008;
}

extern void exc0();
extern void exc1();

extern void irq0();
extern void irq1();

static void __init setup_exc()
{
    void *exc_ptr = exc0;
    unsigned long exc_len = (unsigned long)&exc1 - (unsigned long)&exc0;

    for (int i = 0; i < 32; i++)
    {
        set_idt_entry(i, exc_ptr, INT_PRESENT | INT_GATE | INT_SUPV);
        exc_ptr += exc_len;
    }
}

static void __init setup_irq()
{
    void *irq_ptr = irq0;
    unsigned long irq_len = (unsigned long)&irq1 - (unsigned long)&irq0;

    for (int i = 32; i < 32+16; i++)
    {
        set_idt_entry(i, irq_ptr, INT_PRESENT | INT_GATE | INT_SUPV);
        irq_ptr += irq_len;
    }

    // initialize programmable interrupt controller
    pic_init();
}

static void __init idt_write(struct idt_desc_struct *desc)
{
    __asm__ volatile ("lidt %0" : : "g"(*desc));
}

void __init setup_idt()
{
    struct idt_desc_struct idt_desc;
    idt_desc.size = 8 * IDT_ENTRIES - 1;
    idt_desc.addr = (uint32_t)idt;

    // install exception handlers
    setup_exc();

    setup_irq();

    // don't set handlers for the other one's
    for (int i = 32+16; i < IDT_ENTRIES; i++)
        set_idt_entry(i, NULL, 0);

    // System call interrupt
    //set_idt_entry(0x80, irq_syscall, INT_TRAP | INT_PRESENT | INT_USER);

    // apply IDT and enable interrupts
    idt_write(&idt_desc);
    sti();
}

extern void irq_handler(int);

void irq_handler_i686(uint32_t irq)
{
    // handle interrupt
    irq_handler(irq);

    /* end of interrupt -> notify PIC */
    if (irq >= 32 && irq < 32+16) {
        if (irq >= 32+8)
            outb(0xa0, 0x20);
        outb(0x20, 0x20);
    }
}
