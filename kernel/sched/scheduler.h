#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

typedef enum
{
    SCHED_DISABLED,
    SCHED_ACTIVE,
    SCHED_PAUSED
} scheduler_state_t;

void scheduler_enable(void);
uint32_t schedule(uint32_t esp);

#endif // SCHEDULER_H
