#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "sched/task.h"

pid_t sc_fork(void);
void sc_exit(int status);

pid_t sc_wait(int *wstatus);
pid_t sc_waitpid(pid_t pid, int *wstatus, int options);


#endif // SYSCALLS_H
