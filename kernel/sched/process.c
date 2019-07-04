#include <sched.h>
#include <errno.h>
#include <types.h>

pid_t sys_fork()
{
    // TODO: implement system call
    return -ENOSYS;
}

void sys_exit(int status)
{
    // TODO: implement system call
}

int sys_kill(pid_t pid, int sig)
{
    return -ENOSYS;
}

pid_t sys_getpid(void)
{
    // CAUTION: this function has to be always successful
    return 0;
}

int sys_execve(const char *filename, char *const argv[], char *const envp[])
{
    // TODO: implement system call
    return -ENOSYS;
}

int sys_chdir(const char *path)
{
    return -ENOSYS;
}

int sys_getcwd(char *buf, unsigned long size)
{
    return -ENOSYS;
}

int sys_brk(void *addr)
{
    return -ENOSYS;
}

clock_t sys_times(struct tms *buf)
{
    return -ENOSYS;
}
