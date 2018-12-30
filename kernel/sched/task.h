#ifndef TASK_H
#define TASK_H

#include <util/util.h>
#include <memory/kheap.h>
#include <memory/paging.h>
#include <filesystem/vfscore.h>

#define MAX_FILES 32    // max file descriptors a process can have

typedef signed long int pid_t;

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
    unsigned long ebp;
    unsigned long esp;
    unsigned long kstack;
} kstack_t;

typedef struct _thread_t thread_t;
typedef struct _process_t process_t;

struct _thread_t
{
    pid_t               tid;
    process_t*          process;
    thread_state_t      state;
    int                 priority;

    kstack_t            kstack;

    unsigned long       ksp; // only used if type==kernel
    thread_t*           next_thread;
    char                description[DESC_LENGTH];
};

struct rcontext_struct
{
    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long esp;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
};



kstack_t kstack_init(kstack_t kstack,
                            int thread_type,
                            void *start_addr,
                            unsigned long user_esp,
                            unsigned long eflags,
                            struct rcontext_struct *registers);

kstack_t mk_kstack(thread_type_t thread_type,
                           void *entry,
                           size_t stack_size,
                           unsigned long user_esp,
                           unsigned long eflags,
                           struct rcontext_struct *registers);

thread_t *mk_thread(process_t *process,
                    kstack_t kstack,
                    char *description);

void kill_thread(thread_t *thread);

#endif // TASK_H
