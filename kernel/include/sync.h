#ifndef SYNC_H
#define SYNC_H

struct task_struct
{


    struct task_struct *next_waiting;
};

struct mutex_struct
{
    int locked;
    struct task_struct *waiting;
};

typedef struct mutex_struct mutex_t;

void mutex_init(mutex_t *mtx);
void mutex_lock(mutex_t *mtx);
void mutex_unlock(mutex_t *mtx);

#endif // SYNC_H
