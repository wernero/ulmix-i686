#include <fs.h>
#include <errno.h>

int sys_open(const char *pathname, int flags, mode_t mode)
{
    return -ENOSYS;
}

int sys_close(int fd)
{
    return -ENOSYS;
}

ssize_t sys_read(int fd, void *buf, size_t count)
{
    return -ENOSYS;
}

ssize_t sys_write(int fd, void *buf, size_t count)
{
    return -ENOSYS;
}

off_t sys_lseek(int fd, off_t offset, int whence)
{
    return -ENOSYS;
}

int sys_link(const char *oldpath, const char *newpath)
{
    return -ENOSYS;
}

int sys_unlink(const char *pathname)
{
    return -ENOSYS;
}
