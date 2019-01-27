#include "sync.h"
#include "util/util.h"
#include "memory/kheap.h"
#include "sched/scheduler.h"

extern thread_t *current_thread;
extern volatile scheduler_state_t scheduler_state;

mutex_t *mutex(void)
{
    mutex_t *mtx = kmalloc(sizeof(mutex_t), 1, "mutex_t");
    mtx->blocked = 0;
    mtx->blocker = NULL;
    mtx->blocklist = blocker();
    return mtx;
}

void mutex_lock(mutex_t *mtx)
{
    if (scheduler_state == SCHED_DISABLED)
        return;

    cli();
    if (!mtx->blocked)
    {
        mtx->blocked = 1;
        mtx->blocker = current_thread;
        sti();
    }
    else
    {
        sti();
        blocklist_add(mtx->blocklist);
        mutex_lock(mtx);
    }
}

void mutex_unlock(mutex_t *mtx)
{
    if (mtx->blocked && mtx->blocker == current_thread)
    {
        mtx->blocked = 0;
        blocklist_unblock(mtx->blocklist);
    }
}
