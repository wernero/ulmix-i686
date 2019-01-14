#include "interrupts.h"
#include "log.h"
#include "exceptions.h"

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
extern void irq_syscall(void);

void irq_handler(uint32_t irq)
{
    if (interrupts[irq].unblock != NULL)
    {
        interrupts[irq].unblock->state = RUNNING;
    }

    if (interrupts[irq].handler_count)
    {
        interrupts[irq].handler();
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
    interrupts[id].unblock = NULL;
}


int unblock_on_irq(int irq, thread_t *task)
{
    // TEMPORARY !!!!
    interrupts[irq].unblock = task;
    return 0;
}

void irq_install_raw_handler(int id, void (*handler)(void), int flags)
{
    set_idt_entry(id, handler, flags | INT_PRESENT);
}

void setup_idt(void)
{
    idt_descriptor_t idt_desc;
    idt_desc.size = 8 * IDT_ENTRIES - 1;
    idt_desc.addr = (uint32_t)idt;

    klog(KLOG_INFO, "Interrupt Descriptor Table at address 0x%x", idt_desc.addr);

    uint32_t handler_size =
            (uint32_t)&irq_asm_handler_end -
            (uint32_t)&irq_asm_handler;


    // CPU exception handlers
    int i;
    for (i = 0; i < 32; i++)
    {
        set_idt_entry(i, NULL, 0);
    }
    setup_exception_handlers();


    // IO interrupts
    for (i = 0; i < 16; i++)
    {
        set_idt_entry(i + 32,
                      irq_asm_handler + handler_size * i,
                      INT_GATE | INT_PRESENT | INT_SUPV);
        interrupts[i+32].handler_count = 0;
        interrupts[i+32].executions = 0;
    }


    // custom interrupts have no handler yet
    for (i = 32+16; i < IDT_ENTRIES; i++)
    {
        set_idt_entry(i, NULL, 0);
    }

    // System call interrupt
    set_idt_entry(0x80, irq_syscall, INT_TRAP | INT_PRESENT | INT_USER);

    pic_init();

    klog(KLOG_INFO, "applying IDT (cpu lidt)");
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
