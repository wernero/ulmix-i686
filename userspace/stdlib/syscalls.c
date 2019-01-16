#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

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


int ioctl(int fd, unsigned long request, unsigned long arg) // 54
{
    return __syscall(
        54,
        fd,
        request,
        arg,
        0
    );
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
        _set_errno(-ret);
        return -1;
    }
    return ret;
}
