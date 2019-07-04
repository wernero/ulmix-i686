#include <fs.h>
#include <errno.h>

int sys_stat(const char *pathname, struct stat *statbuf)
{
    return -ENOSYS;
}

int sys_fstat(int fd, struct stat *statbuf)
{
    return -ENOSYS;
}
