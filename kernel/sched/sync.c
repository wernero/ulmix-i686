#include "sync.h"
#include "util/util.h"
#include "memory/kheap.h"
#include "sched/scheduler.h"

extern thread_t *current_thread;
extern volatile scheduler_state_t scheduler_state;

mutex_t *mutex(void)
{
    /*mutex_t *mtx = kmalloc(sizeof(mutex_t), 1, "mutex_t");
    mtx->blocked = 0;
    mtx->blocker = NULL;
    mtx->blocklist = blocker();
    return mtx;*/
    return NULL;
}

void mutex_lock(mutex_t *mutex)
{
    /*cli();
    scheduler_state_t tmp = scheduler_state;
    scheduler_state = SCHED_PAUSED;
    sti();

    if (!mutex->blocked)
    {
        mutex->blocked = 1;
        mutex->blocker = current_thread;
    }
    else
    {
        blocklist_add(mutex->blocklist, current_thread);
    }

    scheduler_state = tmp;*/
}

void mutex_unlock(mutex_t *mutex)
{
    if (!mutex->blocked)
        return;

    mutex->blocked = 0;

    // unblock thread(s)
}
