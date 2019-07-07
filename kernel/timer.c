#include <time.h>
#include <errno.h>
#include <types.h>
#include <interrupt.h>
#include <asm.h>

#define TIMER_IRQ 32

#define IRQ_TIMER   0x20        // IRQ 0 on PIC1
#define OSCILLATOR  1193182     // constant
#define TIMER_FREQ  100         // in Hz

#define PIT_CMD     0x43
#define PIT_CH0     0x40
#define PIT_CMD_CH0 0x00
#define PIT_ACCESS  0x30
#define PIT_RATEGEN 0x04

unsigned long timer_ticks = 0;

static void irq_timer(void)
{
    timer_ticks++;
}

void setup_timer()
{
    // setup timer
    uint16_t pit = OSCILLATOR / TIMER_FREQ;
    outb(PIT_CMD, PIT_CMD_CH0 | PIT_ACCESS | PIT_RATEGEN);
    outb(PIT_CH0, pit & 0xff);
    outb(PIT_CH0, (pit >> 8) & 0xff);

    // Register IRQ
    register_irq(TIMER_IRQ, irq_timer);
}

int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
    return -ENOSYS;
}
