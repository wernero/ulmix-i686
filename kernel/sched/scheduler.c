#include "scheduler.h"
#include "util/util.h"
#include "memory/gdt.h"
#include "log.h"

volatile scheduler_state_t scheduler_state = SCHED_DISABLED;

static task_queue_t *running_tasks = NULL;

extern thread_t *current_thread;
static thread_t *get_next_task(void);
static void idle_task(void);

void scheduler_enable()
{
    if (scheduler_state == SCHED_DISABLED)
    {
        mk_kernel_thread(mk_kstack(TYPE_KERNEL, idle_task, 1024, 0, get_eflags()), "idle task");
        klog(KLOG_INFO, "scheduler_enable(): activating Multitasking");
    }
    scheduler_state = SCHED_ACTIVE;
}

void scheduler_disable(void)
{
    scheduler_state = SCHED_PAUSED;
}

void scheduler_remove(thread_t *thread)
{
    // TODO: actually remove from list; free() resources
    thread->state = KILLED;
}

void scheduler_force(void)
{
    // raise timer interrupt
    __asm__("int $0x20");
}

static void _insert(thread_t *thread, task_queue_t **queue)
{
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
}

void scheduler_insert(thread_t *thread)
{
    // TODO: make thread safe (!!!!)

    _insert(thread, &running_tasks);
}

extern volatile uint32_t timer_ticks;
uint32_t schedule(uint32_t esp)
{
    if (timer_ticks % 400 != 0)
        return esp;

    if (current_thread != NULL)
        current_thread->kstack.esp = esp;


    thread_t *next = get_next_task();
    if (current_thread != next)
    {
        klog(KLOG_DEBUG, "restoring %s", next->description);
        current_thread = next;
        update_tss(next->kstack.ebp);

        if (current_thread->process != NULL)
        {
            apply_pagedir(current_thread->process->pagedir);
        }
    }

    return current_thread->kstack.esp;
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

static void idle_task()
{
    for (;;) hlt();
}
