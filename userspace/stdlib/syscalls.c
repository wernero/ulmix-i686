#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

static unsigned long __syscall(
        unsigned long    id,
        unsigned long    arg1,
        unsigned long    arg2,
        unsigned long    arg3,
        unsigned long    arg4);

int _exit(int err) // 1
{
    return __syscall(1, err, 0, 0, 0);
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

// waitpid() // 7
// creat() // 8
// link() // 9
// unlink() // 10

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
static unsigned long __syscall(
        unsigned long    id,
        unsigned long    arg1,
        unsigned long    arg2,
        unsigned long    arg3,
        unsigned long    arg4)
{
    unsigned long ret;
    __asm__(
        "mov %1, %%eax;"
        "mov %2, %%ebx;"
        "mov %3, %%ecx;"
        "mov %4, %%edx;"
        "mov %5, %%esi;"
        "int $0x80;"
        "mov %%eax, %0"
        :
        "=g"(ret)
        :
        "g"(id),
        "g"(arg1),
        "g"(arg2),
        "g"(arg3),
        "g"(arg4)
    );

    if (ret < 0)
        _set_errno(-ret);
    return ret;
}
