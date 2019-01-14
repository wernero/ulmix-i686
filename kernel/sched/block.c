#include "block.h"
#include "interrupts.h"
#include "memory/kheap.h"

extern thread_t *current_thread;




blocklist_t *blocker(void)
{
    blocklist_t *list = kmalloc(sizeof(blocklist_t), 1, "blocklist_t");
    list->entries = 0;
    return list;
}

void blocklist_add(blocklist_t *blocklist)
{
    scheduler_block(current_thread);
    blocklist->threads[blocklist->entries++] = current_thread;
}

void blocklist_unblock(blocklist_t *blocklist)
{
    for (int i = 0; i < blocklist->entries; i++)
    {
        scheduler_unblock(blocklist->threads[i]);
    }
    blocklist->entries = 0;
}


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
