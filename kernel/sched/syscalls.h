#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sched/task.h>

enum woptions_enum
{
    WHANG       = 0x01,
    WUNTRACED   = 0x02,
    WCONTINUED  = 0x04
};

#define WIFEXITED       0x01
#define WEXITSTATUS     0x02
#define WIFSIGNALED     0x04
#define WTERMSTATUS     0x08
#define WCOREDUMP       0x10
#define WIFSTOPPED      0x20
#define WSTOPSIG        0x40
#define WIFCONTINUED    0x80

pid_t sys_fork(void);
void sys_exit(int status);

pid_t sys_waitpid(pid_t pid, int *wstatus, int options);


#endif // SYSCALLS_H
