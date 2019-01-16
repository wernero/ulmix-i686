#include "fs_syscalls.h"
#include "filesystem.h"
#include "path.h"
#include <drivers/devices.h>
#include <errno.h>
#include <sched/task.h>
#include <sched/process.h>
#include <video/tty.h>
#include <kdebug.h>

extern thread_t *current_thread;

int sc_creat(const char *pathname, int mode)
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

int sc_open(char *pathname, int flags)
{
    struct direntry_struct *node;
    if (namei(pathname, &node) < 0)
        return -ENOENT;

    if (node->type == REGULAR)
        return open_file(node, flags);

    // path is not a regular file
    return -EIO;
}

ssize_t sc_write(int fd, void *buf, size_t count)
{
    if (fd == 912)
    {
        // debug !!!
        return tty_kernel_write(buf, count);
    }

    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL || fds->fops.write == NULL)
        return -EBADF;

    return fds->fops.write(fds, buf, count);
}

ssize_t sc_read(int fd, void *buf, size_t count)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL || fds->fops.read == NULL)
        return -EBADF;

    return fds->fops.read(fds, buf, count);
}

int sc_close(int fd)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL || fds->fops.close == NULL)
        return -EBADF;

    return fds->fops.close(fds);
}

ssize_t sc_lseek(int fd, size_t offset, int whence)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;

    return fds->fops.seek(fds, offset, whence);
}
