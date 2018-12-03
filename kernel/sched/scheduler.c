#include "scheduler.h"

scheduler_state_t scheduler_state = SCHED_DISABLED;

extern thread_t *current_thread;

void scheduler_enable()
{
    scheduler_state = SCHED_ACTIVE;
}

uint32_t schedule(uint32_t esp)
{
    current_thread->kstack.esp = esp;


    return current_thread->kstack.esp;
}
