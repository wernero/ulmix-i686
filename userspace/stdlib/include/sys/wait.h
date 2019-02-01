#ifndef WAIT_H
#define WAIT_H

#include <sys/types.h>

enum woptions_enum
{
    WHANG       = 0x01,
    WUNTRACED   = 0x02,
    WCONTINUED  = 0x04
};

#define FL_WIFEXITED       0x01
#define FL_WEXITSTATUS     0x02
#define FL_WIFSIGNALED     0x04
#define FL_WTERMSTATUS     0x08
#define FL_WCOREDUMP       0x10
#define FL_WIFSTOPPED      0x20
#define FL_WSTOPSIG        0x40
#define FL_WIFCONTINUED    0x80

#define WIFEXITED(a)    (((a) & FL_WIFEXITED)    ? 1 : 0)
#define WEXITSTATUS(a)  (((a) & FL_WEXITSTATUS)  ? 1 : 0)
#define WIFSIGNALED(a)  (((a) & FL_WIFSIGNALED)  ? 1 : 0)
#define WTERMSTATUS(a)  (((a) & FL_WTERMSTATUS)  ? 1 : 0)
#define WCOREDUMP(a)    (((a) & FL_WCOREDUMP)    ? 1 : 0)
#define WIFSTOPPED(a)   (((a) & FL_WIFSTOPPED)   ? 1 : 0)
#define WSTOPSIG(a)     (((a) & FL_WSTOPSIG)     ? 1 : 0)
#define WIFCONTINUED(a) (((a) & FL_WIFCONTINUED) ? 1 : 0)

pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);

// int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

#endif // WAIT_H
