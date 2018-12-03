#include "filesystem.h"


void vfs_init()
{

}

// Stubs for System calls

int sc_open(const char *pathname, int flags)
{
    return -1;
}

int sc_creat(const char *pathname, mode_t mode)
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
