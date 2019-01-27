#include "open.h"

#include <sched/task.h>
#include <sched/process.h>
#include <devices/devices.h>
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


static int open_char(struct file_struct *fd, int major, int minor, int flags)
{
    struct chardev_struct *cd = find_chardev(major, minor);
    if (cd == NULL)
        return -ENODEV;

    fd->fops = cd->fops;
    fd->drv.cd = cd;

    if (fd->fops.open == NULL)
        return SUCCESS;
    return fd->fops.open(fd, flags, fd->drv);
}

static int open_blk(struct file_struct *fd, int major, int minor, int flags)
{
    struct gendisk_struct *bd = find_gendisk(major, minor);
    if (bd == NULL)
        return -ENODEV;

    fd->fops = bd->fops;
    fd->drv.bd = bd;

    fd->lock_offset = bd->offset;
    if (bd->capacity > 0)
        fd->lock_size = bd->capacity;

    if (fd->fops.open == NULL)
        return SUCCESS;
    return fd->fops.open(fd, flags, fd->drv);
}

struct file_struct *kopen_device(ftype_t type, int major, int minor, int flags)
{
    struct file_struct *fd = kmalloc(sizeof(struct file_struct), 1, "file_struct device");
    fd->direntry = NULL;
    fd->open_mode = flags;
    fd->seek_offset = 0;

    int ret;
    if (type == CHARDEVICE)
        ret = open_char(fd, major, minor, flags);
    else if (type == BLOCKDEVICE)
        ret = open_blk(fd, major, minor, flags);
    else
        ret = -EINVAL;

    if (ret < 0)
    {
        kfree(fd);
        return NULL;
    }
    return fd;
}

void kclose(struct file_struct *fd)
{
    kfree(fd);
}

int open_device(struct direntry_struct *node, int flags)
{
    // somehow, major/minor are only 8 bit???
    int major = (node->bptr1 & 0xff00) >> 8;
    int minor = node->bptr1 & 0xff;

    struct file_struct *fd = kmalloc(sizeof(struct file_struct), 1, "file_struct device");
    fd->direntry = node;
    fd->open_mode = flags;
    fd->seek_offset = 0;

    int ret;
    if (node->type == CHARDEVICE)
        ret = open_char(fd, major, minor, flags);
    else if (node->type == BLOCKDEVICE)
        ret = open_blk(fd, major, minor, flags);
    else
        ret = -EINVAL;

    if (ret < 0)
    {
        kfree(fd);
        return ret;
    }
    return insert_fd(fd);
}
