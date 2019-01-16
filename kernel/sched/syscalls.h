#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "sched/task.h"

pid_t sys_fork(void);
void sys_exit(int status);

pid_t sys_wait(int *wstatus);
pid_t sys_waitpid(pid_t pid, int *wstatus, int options);


#endif // SYSCALLS_H
