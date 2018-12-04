#ifndef SYNC_H
#define SYNC_H

#include "sched/task.h"

typedef struct
{
    thread_t *blocker;
    int blocked;
} mutex_t;

mutex_t *mutex(void);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

#endif // SYNC_H
