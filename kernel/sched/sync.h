#ifndef SYNC_H
#define SYNC_H

#include "task.h"
#include "block.h"

typedef struct
{
    thread_t *blocker;
    int blocked;
    blocklist_t *blocklist;
} mutex_t;

mutex_t *mutex(void);
void mutex_lock(mutex_t *mtx);
void mutex_unlock(mutex_t *mtx);

#endif // SYNC_H
