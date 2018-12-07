#include "fs_syscalls.h"
#include "filesystem/path.h"
#include "filesystem/vfscore.h"

#define O_APPEND    0x01
#define O_CREAT     0x02
#define O_RDONLY    0x04
#define O_RDWR      0x08
#define O_WRONLY    0x10

int sc_open(const char *pathname, int flags)
{
    return -1;
}

int sc_creat(const char *pathname, int mode)
{
    return -1;
}

ssize_t sc_write(int fd, const void *buf, size_t count)
{
    return -1;
}

ssize_t sc_read(int fd, const void *buf, size_t count)
{
    return -1;
}

int sc_close(int fd)
{
    return -1;
}

int sc_link(const char *oldpath, const char *newpath)
{
    return -1;
}

int sc_unlink(const char *pathname)
{
    return -1;
}

ssize_t sc_lseek(int fd, size_t offset, int whence)
{
    return -1;
}
