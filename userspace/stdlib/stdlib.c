#include <stdlib.h>
#include <syscalls.h>

pid_t fork(void)
{
    return sysc(1, 0, 0, 0, 0);
}
