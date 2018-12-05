#ifndef BLOCK_H
#define BLOCK_H

#include "sched/task.h"
#include "sched/scheduler.h"

typedef struct _blocklist blocklist_t;
struct _blocklist
{
    thread_t *threads[20];
    int entries;
};

blocklist_t *blocker(void);
void blocklist_add(blocklist_t *blocklist, thread_t *task);
void blocklist_unblock(blocklist_t *blocklist);

int waitirq(int irq);

#endif // BLOCK_H
