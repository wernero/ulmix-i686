#include <sync.h>
#include <asm.h>

void mutex_init(mutex_t *mtx)
{
    mtx->locked = 0;
    mtx->waiting = NULL;
}

void mutex_lock(mutex_t *mtx)
{
    check_mutex:
    //cli();

    if (mtx->locked)
    {
        // suspend_task(current_task);
        //sti();
        // schedule();

        goto check_mutex;
    }

    mtx->locked = 1;
    //sti();
}

void mutex_unlock(mutex_t *mtx)
{
    //cli();

    mtx->locked = 0;

    if (mtx->waiting)
    {
        // resume_task(mtx->waiting);
        mtx->waiting = mtx->waiting->next_waiting;
    }

    //sti();
}
