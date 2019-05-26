#include "timer.h"
#include "util/util.h"
#include "util/types.h"
#include "sched/scheduler.h"
#include "log.h"

#define IRQ_TIMER   0x20        // IRQ 0 on PIC1
#define OSCILLATOR  1193182     // constant
#define TIMER_FREQ  100           // in Hz

#define PIT_CMD     0x43
#define PIT_CH0     0x40
#define PIT_CMD_CH0 0x00
#define PIT_ACCESS  0x30
#define PIT_RATEGEN 0x04

/* in irq.asm: calls irq_timer() */
extern void irq_asm_timer(void);
extern scheduler_state_t scheduler_state;

volatile unsigned long timer_ticks = 0;
unsigned long irq_timer(uint32_t esp)
{
    timer_ticks++;

    if (scheduler_state == SCHED_ACTIVE)
    {
        esp = schedule(esp);
    }

    outb(0x20, 0x20);
    return esp;
}

void setup_timer()
{
    /*klog(KLOG_INFO, "IRQ0 Timer @ %dHz", TIMER_FREQ);
    irq_install_raw_handler(IRQ_TIMER,
                            irq_asm_timer,
                            INT_GATE | INT_SUPV);


    uint16_t pit = OSCILLATOR / TIMER_FREQ;
    outb(PIT_CMD, PIT_CMD_CH0 | PIT_ACCESS | PIT_RATEGEN);
    outb(PIT_CH0, pit & 0xff);
    outb(PIT_CH0, (pit >> 8) & 0xff);*/
}
