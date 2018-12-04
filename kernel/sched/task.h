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
    ZOMBIE,
    SUSPENDED
} thread_state_t;

typedef enum
{
    TYPE_KERNEL,
    TYPE_USER
} thread_type_t;

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
    thread_type_t       type;
    int                 priority;

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

process_t *         mk_process(pagedir_t *pagedir, void (*entry)(void), uint32_t esp, char *description);
thread_t *          mk_kernel_thread(process_kstack_t kstack, char *description);
process_kstack_t    kstack_init(process_kstack_t kstack, int thread_type, void *start_addr, uint32_t user_esp, uint32_t eflags);
process_kstack_t    mk_kstack(thread_type_t thread_type, void *entry, size_t stack_size, uint32_t user_esp, uint32_t eflags);
void                kill_thread(thread_t *thread);

#endif // TASK_H
