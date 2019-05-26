#include "scheduler.h"
#include "process.h"
#include <util/util.h>
#include <kdebug.h>

#include <mem.h>

volatile scheduler_state_t scheduler_state = SCHED_DISABLED;

static task_queue_t *running_tasks = NULL;

extern thread_t *current_thread;
static thread_t *get_next_task(void);

void scheduler_enable()
{
    if (scheduler_state == SCHED_DISABLED)
    {
        klog(KLOG_INFO, "scheduler_enable(): activating Multitasking");
    }
    scheduler_state = SCHED_ACTIVE;
}

void scheduler_disable(void)
{
    scheduler_state = SCHED_PAUSED;
}

static thread_t *get_next_task()
{
    task_queue_t *next_task = running_tasks->next_task;
    while (next_task->task->state != RUNNING)
    {
        next_task = next_task->next_task;
    }
    return (running_tasks = next_task)->task;
}

void scheduler_remove(thread_t *thread)
{
    thread->state = KILLED;
}

void scheduler_force(void)
{
    // raise timer interrupt -> force task switch
    __asm__("int $0x20");
}

static void _insert(thread_t *thread, task_queue_t **queue)
{
    cli();
    scheduler_state_t tmp = scheduler_state;
    scheduler_state = SCHED_PAUSED;
    sti();

    task_queue_t *new_entry = kmalloc(sizeof(task_queue_t), 1, "task_queue_t");
    new_entry->task = thread;
    new_entry->next_task = new_entry;

    if (*queue == NULL)
    {
        *queue = new_entry;
    }
    else
    {
        new_entry->next_task = (*queue)->next_task;
        (*queue)->next_task  = new_entry;
    }

    scheduler_state = tmp;
}

void scheduler_insert(thread_t *thread)
{
    // TODO: make thread safe (!!!!)

    _insert(thread, &running_tasks);
}

unsigned long schedule(unsigned long esp)
{
    if (current_thread != NULL)
        current_thread->kstack.esp = esp;

    thread_t *next = get_next_task();
    if (current_thread != next)
    {
        if (current_thread != NULL &&
                next->process->pagedir != current_thread->process->pagedir)
            apply_pagedir(next->process->pagedir);

        update_tss(next->kstack.ebp);
        current_thread = next;
    }

    return current_thread->kstack.esp;
}

void scheduler_block(thread_t *thread)
{
    thread->state = SUSPENDED;
    if (thread == current_thread)
        scheduler_force();
}

void scheduler_unblock(thread_t *thread)
{
    thread->state = RUNNING;
}

void idle_task()
{
    for (;;) hlt();
}
