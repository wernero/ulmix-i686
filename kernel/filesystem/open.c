#include "open.h"

#include <sched/task.h>
#include <sched/process.h>
#include <drivers/devices.h>
#include <kdebug.h>
#include <errno.h>

extern thread_t *current_thread;
static int insert_fd(struct file_struct *fd)
{
    process_t *p = current_thread->process;

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (p->files[i] == NULL)
        {
            p->files[i] = fd;
            return i;
        }
    }

    return -ENOBUFS;
}

int open_file(struct direntry_struct *node, int flags)
{
    // create file descriptor
    struct file_struct *fd = kmalloc(sizeof(struct file_struct), 1, "sys_open(2) file_struct");
    fd->direntry = node;
    fd->open_mode = flags;
    fd->seek_offset = 0;

    struct fd_fops_struct fops;
    fops.close = NULL;
    fops.read = fd->direntry->parent->sb->fs->fs_read;
    fops.write = fd->direntry->parent->sb->fs->fs_write;
    fops.seek = fd->direntry->parent->sb->fs->fs_seek;

    fd->fops = fops;

    return insert_fd(fd);
}

int open_by_major(int major, int minor, int flags, struct direntry_struct *node)
{
    struct file_struct *fd = kmalloc(sizeof(struct file_struct), 1, "file_struct device");
    fd->direntry = node;
    fd->open_mode = flags;
    fd->seek_offset = 0;

    struct chardev_struct *cd = find_chardev(major);
    if (cd == NULL)
        return -ENODEV;

    fd->fops = cd->fops;

    int ret;
    if ((ret = fd->fops.open(fd, flags, minor)) < 0)
    {
        kfree(fd);
        return ret;
    }

    return insert_fd(fd);
}

int open_device(struct direntry_struct *node, int flags)
{
    if (node->type != CHARDEVICE && node->type != BLOCKDEVICE)
        return -EIO;

    // somehow, major/minor are only 8 bit???
    int major = (node->bptr1 & 0xff00) >> 8;
    int minor = node->bptr1 & 0xff;

    return open_by_major(major, minor, flags, node);
}
