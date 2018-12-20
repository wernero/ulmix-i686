#ifndef SYSCALL_H
#define SYSCALL_H

#include "sched/task.h"

pid_t sc_fork(void);
void sc_exit(int status);
pid_t sc_wait(int *wstatus);
pid_t sc_waitpid(pid_t pid, int *wstatus, int options);

struct syscall_context_struct
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t eip;
};

#endif // SYSCALL_H
