#include "syscalls.h"


pid_t sc_fork(void)
{
    return -1;
}

void sc_exit(int status)
{
}

pid_t sc_wait(int *wstatus)
{
    return -1;
}

pid_t sc_waitpid(pid_t pid, int *wstatus, int options)
{
    return -1;
}
