#ifndef SCHED_H
#define SCHED_H

typedef long pid_t;
typedef long clock_t;
typedef unsigned long clock_user_t;

struct tms
{
    clock_user_t tms_utime;
    clock_user_t tms_stime;
    clock_user_t tms_cutime;
    clock_user_t tms_cstime;
};

void    sys_exit(int status);
pid_t   sys_fork(void);
pid_t   sys_getpid(void);
int     sys_kill(pid_t pid, int sig);
int     sys_execve(const char *filename, char *const argv[],
                   char *const envp[]);
int     sys_getcwd(char *buf, unsigned long size);
int     sys_chdir(const char *path);
int     sys_brk(void *addr);
clock_t sys_times(struct tms *buf);

#endif // SCHED_H
