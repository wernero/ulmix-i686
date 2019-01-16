#include "fs_syscalls.h"
#include "filesystem.h"
#include "open.h"
#include "path.h"
#include <drivers/devices.h>
#include <errno.h>
#include <sched/task.h>
#include <sched/process.h>
#include <video/tty.h>
#include <kdebug.h>

extern thread_t *current_thread;
static struct file_struct *get_fd(int fd)
{
    struct file_struct *fds = current_thread->process->files[fd];
    klog(KLOG_INFO, "obtained fd %d -> 0x%x", fd, fds);
    return fds;
}

int sys_creat(const char *pathname, int mode)
{
    return -1;
}

int sys_link(const char *oldpath, const char *newpath)
{
    return -1;
}

int sys_unlink(const char *pathname)
{
    return -1;
}

int sys_open(char *pathname, int flags)
{
    struct direntry_struct *node;
    if (namei(pathname, &node) < 0)
        return -ENOENT;

    klog(KLOG_INFO, "open(): node->type = %x", node->type);

    if (node->type == REGULAR)
        return open_file(node, flags);

    if (node->type == CHARDEVICE)
        return open_device(node, flags);

    // path is not a regular file
    return -EIO;
}

ssize_t sys_write(int fd, void *buf, size_t count)
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

ssize_t sys_read(int fd, void *buf, size_t count)
{
    struct file_struct *fds = get_fd(fd);
    if (fds == NULL || fds->fops.read == NULL)
        return -EBADF;

    return fds->fops.read(fds, buf, count);
}

int sys_close(int fd)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL || fds->fops.close == NULL)
        return -EBADF;

    return fds->fops.close(fds);
}

ssize_t sys_lseek(int fd, size_t offset, int whence)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;

    return fds->fops.seek(fds, offset, whence);
}
