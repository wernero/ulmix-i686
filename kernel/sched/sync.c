#include "sync.h"
#include "util/util.h"
#include "memory/kheap.h"

extern thread_t *current_task;

mutex_t *mutex(void)
{
    mutex_t *mtx = kmalloc(sizeof(mutex_t), 1, "mutex_t");
    mtx->blocked = 0;
    mtx->blocker = NULL;
    return mtx;
}

void mutex_lock(mutex_t *mutex)
{

}

void mutex_unlock(mutex_t *mutex)
{

}
