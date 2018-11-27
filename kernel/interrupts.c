#include "interrupts.h"
#include "log.h"
#include "video/video.h"

idt_entry_t idt[IDT_ENTRIES];
interrupt_t interrupts[IDT_ENTRIES];

static void set_idt_entry(int id, void (*handler)(void), int flags);
static void pic_init(void);

void irq_handler(uint32_t irq);
void irq_timer(void);

/* irq.asm */
extern void idt_write(idt_descriptor_t *desc);
extern void irq_asm_handler(void);
extern char irq_asm_handler_end;


static const char* const exceptions[] =
{
    "divide by zero",          "debug",                         "non-maskable interrupt",    "breakpoint",
    "into detected overflow",  "out of Bounds",                 "invalid opcode",            "no coprocessor",
    "double fault",            "coprocessor Segment Overrun",   "broken task state segment", "segment not present",
    "stack fault",             "general Protection Fault",      "page fault",                "unknown interrupt",
    "coprocessor fault",       "alignment Check",               "machine check",             "SIMD exception",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved",
    "reserved",                "reserved",                      "reserved",                  "reserved"
};

void irq_handler(uint32_t irq)
{
    if (irq < 32)
    {
        kprintf("exception: %s\n", exceptions[irq]);
    }

    if (interrupts[irq].handler_count)
    {
        interrupts[irq].handler();
    }
    else
    {
        kprintf("unknown interrupt #%d\n", irq);
    }

    /* end of interrupt -> PIC */
    if (irq >= 32 && irq < 32+16)
    {
        if (irq >= 32+8)
            outb(0xa0, 0x20);
        outb(0x20, 0x20);
    }
}

static void set_idt_entry(int id, void (*handler)(void), int flags)
{
    idt[id].offset_low   = (uint32_t)handler & 0x0000ffff;
    idt[id].offset_high  = ((uint32_t)handler & 0xffff0000) >> 16;
    idt[id].zero         = 0x00;
    idt[id].type         = flags;
    idt[id].selector     = 0x0008;
}

void irq_install_handler(int id, void (*handler)(void))
{
    interrupts[id].handler = handler;
    interrupts[id].handler_count++;
}

void irq_install_raw_handler(int id, void (*handler)(void), int flags)
{
    cli();
    set_idt_entry(id, handler, flags | INT_PRESENT);
    sti();
}

void setup_idt(void)
{
    idt_descriptor_t idt_desc;
    idt_desc.size = 8 * IDT_ENTRIES - 1;
    idt_desc.addr = (uint32_t)idt;

    uint32_t handler_size =
            (uint32_t)&irq_asm_handler_end -
            (uint32_t)&irq_asm_handler;

    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        set_idt_entry(i,
                      irq_asm_handler + handler_size * i,
                      INT_GATE | INT_PRESENT | INT_SUPV);
        interrupts[i].handler_count = 0;
    }

    //set_idt_entry(0x80, irq_asm_syscall, INT_GATE | INT_PRESENT | INT_USER);

    pic_init();
    idt_write(&idt_desc);
}


static void pic_init()
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
