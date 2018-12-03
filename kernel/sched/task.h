#ifndef TASK_H
#define TASK_H

#include "util/util.h"
#include "memory/kheap.h"
#include "memory/paging.h"

typedef unsigned long pid_t;

typedef enum
{
    RUNNING,
    KILLED,
    SUSPENDED
} thread_state_t;

typedef struct
{
    uint32_t ebp;
    uint32_t esp;
    uint32_t kstack;
} process_kstack_t;

typedef struct _thread_t thread_t;
typedef struct _process_t process_t;

struct _thread_t
{
    pid_t               tid;
    process_t*          process;
    thread_state_t      state;

    process_kstack_t    kstack;

    thread_t*           next_thread;
    char                description[DESC_LENGTH];
};

struct _process_t
{
    pid_t           pid;
    pid_t           ppid;
    pagedir_t*      pagedir;
    thread_t*       threads;
    char            description[DESC_LENGTH];
};

process_t *mk_process(pagedir_t *pagedir, void (*entry)(void), uint32_t esp, char *description);

#endif // TASK_H
