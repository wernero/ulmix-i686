/* ulibc - Ulmix C Library
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>

int errno;

static unsigned long
__attribute__((optimize("O0")))
__syscall(unsigned long id,
          unsigned long arg1,
          unsigned long arg2,
          unsigned long arg3,
          unsigned long arg4);

void _exit(int err) // 1
{
    __syscall(1, err, 0, 0, 0);
}

pid_t fork(void) // 2
{
    return __syscall(2, 0, 0, 0, 0);
}

ssize_t read(int fd, void *buf, size_t len) // 3
{
    return __syscall(3, fd, (unsigned long)buf, len, 0);
}

ssize_t write(int fd, void *buf, size_t len) // 4
{
    return __syscall(4, fd, (unsigned long)buf, len, 0);
}

int open(char *path, int flags) // 5
{
    return __syscall(5, (unsigned long)path, flags, 0, 0);
}

int close(int fd) // 6
{
    return __syscall(6, fd, 0, 0, 0);
}

pid_t waitpid(pid_t pid, int *wstatus, int options) // 7
{
    return __syscall(7, pid, (unsigned long)wstatus, (unsigned long)options, 0);
}

// creat() // 8
// link() // 9
// unlink() // 10

int chdir(const char *path) // 12
{
    return __syscall(12, (unsigned long)path, 0, 0, 0);
}

int kdebug(unsigned long request, unsigned long arg1, unsigned long arg2) // 17
{
    return __syscall(17, request, arg1, arg2, 0);
}

pid_t getpid(void) // 20
{
    return __syscall(20, 0, 0, 0, 0);
}

int uname(struct utsname *buf) // 109
{
    return __syscall(109, (unsigned long)buf, 0, 0, 0);
}

char *getcwd(char *buf, size_t size) // 183
{
    if (__syscall(183, (unsigned long)buf, size, 0, 0) == 0)
        return buf;
    return NULL;
}

int execve(const char *filename,
           char *const argv[],
           char *const envp[]) // 11
{
    return __syscall(11,
             (unsigned long)filename,
             (unsigned long)argv,
             (unsigned long)envp, 0);
}

// actual syscall implementation
static unsigned long
__attribute__((optimize("O0")))
__syscall(unsigned long id,
          unsigned long arg1,
          unsigned long arg2,
          unsigned long arg3,
          unsigned long arg4)
{
    unsigned long ret;
    __asm__(
        "int $0x80;"
        :
        "=a"(ret)
        :
        "a"(id),
        "b"(arg1),
        "c"(arg2),
        "d"(arg3),
        "S"(arg4)
    );

    if (ret < 0)
    {
        errno = -ret;
        return -1;
    }
    return ret;
}
