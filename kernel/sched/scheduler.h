#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

typedef enum
{
    SCHED_DISABLED,
    SCHED_ACTIVE,
    SCHED_PAUSED
} scheduler_state_t;

typedef struct _task_queue task_queue_t;
struct _task_queue
{
    thread_t *task;
    task_queue_t *next_task;
};

void scheduler_enable(void);
void scheduler_disable(void);
void scheduler_insert(thread_t *thread);
void scheduler_remove(thread_t *thread);
void scheduler_force(void);
uint32_t schedule(uint32_t esp);

#endif // SCHEDULER_H
