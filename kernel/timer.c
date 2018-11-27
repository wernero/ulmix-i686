#include "timer.h"
#include "interrupts.h"
#include "util/util.h"
#include "util/types.h"

#include "video/video.h" // temp

#define IRQ_TIMER   0x20        // IRQ 0 on PIC1
#define OSCILLATOR  1193182     // constant
#define TIMER_FREQ  200         // in Hz

#define PIT_CMD     0x43
#define PIT_CH0     0x40
#define PIT_CMD_CH0 0x00
#define PIT_ACCESS  0x30
#define PIT_RATEGEN 0x04

/* in irq.asm: calls irq_timer() */
extern void irq_asm_timer(void);

uint32_t timer_ticks = 0;
void irq_timer(void)
{
    timer_ticks++;
    if (timer_ticks % 200 == 0)
        kprintf("1 sec\n");
}

void setup_timer()
{
    irq_install_raw_handler(IRQ_TIMER,
                            irq_asm_timer,
                            INT_GATE | INT_SUPV);

    uint16_t pit = OSCILLATOR / TIMER_FREQ;
    outb(PIT_CMD, PIT_CMD_CH0 | PIT_ACCESS | PIT_RATEGEN);
    outb(PIT_CH0, pit & 0xff);
    outb(PIT_CH0, (pit >> 8) & 0xff);
}
