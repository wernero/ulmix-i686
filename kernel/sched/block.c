#include "block.h"
#include "interrupts.h"

extern thread_t *current_thread;

int waitirq(int irq)
{
    if (irq < 0 || irq > 255)
    {
        return -1;
    }

    unblock_on_irq(irq, current_thread);
    current_thread->state = SUSPENDED;
    scheduler_force();

    return 0;
}
