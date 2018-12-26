#include <syscalls.h>
#include <sys/types.h>
#include <errno.h>

static unsigned long __syscall(
        unsigned int    id,
        unsigned int    arg1,
        unsigned int    arg2,
        unsigned int    arg3,
        unsigned int    arg4);

void exit(void)
{
    // id = 1
    __syscall(1, 0, 0, 0, 0);
}

int open(char *path, int flags)
{
    return -ENOSYS;
}

int close(int fd)
{
    return -ENOSYS;
}

ssize_t read(int fd, char *buf, size_t len)
{
    return -ENOSYS;
}

ssize_t write(int fd, char *buf, size_t len)
{
    return -ENOSYS;
}

// actual syscall implementation
static unsigned long __syscall(
        unsigned int    id,
        unsigned int    arg1,
        unsigned int    arg2,
        unsigned int    arg3,
        unsigned int    arg4)
{
    unsigned long ret;
    __asm__(
        "mov %0, %%eax;"
        "mov %1, %%ebx;"
        "mov %2, %%ecx;"
        "mov %3, %%edx;"
        "mov %4, %%esi;"
        "int $0x80;"
        "mov %%eax, %0"
        :
        "=r"(ret)
        :
        "g"(id),
        "g"(arg1),
        "g"(arg2),
        "g"(arg3),
        "g"(arg4)
    );
    return ret;
}
