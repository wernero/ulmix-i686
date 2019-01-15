#include "filesystem.h"
#include "ext2.h"
#include <memory/kheap.h>
#include <drivers/devices.h>
#include <sched/task.h>
#include <sched/process.h>
#include <kdebug.h>
#include <errno.h>

#define SUP_FS_COUNT 10
struct filesystem_struct *supported_filesystems[SUP_FS_COUNT];

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

void init_filesystems()
{
    for (int i = 0; i < SUP_FS_COUNT; i++)
        supported_filesystems[i] = NULL;

    install_ext2();
}

int install_fs(struct filesystem_struct *fs)
{
    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        if (supported_filesystems[i] == NULL)
        {
            supported_filesystems[i] = fs;
            return 0;
        }
    }

    return -1;
}

int direntry_get_inode(struct direntry_struct *file)
{
    return file->parent->sb->fs->fs_get_inode(file, file->inode_no);  /// get_inode function changed !!!!
}

int direntry_get_dir(struct dir_struct *dir)
{
    return dir->sb->fs->fs_get_direntry(dir);
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
    fops.seek = NULL;

    fd->fops = fops;

    node->fd = fd;      // there can be multiple file descriptors!

    if ((flags | O_WRONLY) || (flags | O_RDWR) || (flags | O_APPEND))
    {
        // MUTEX!!!
        if (node->read_opens > 0 || node->write_opens > 0)
        {
            return -EMFILE;
        }

        node->write_opens++;
    }
    else
    {
        // MUTEX!!!
        if (node->write_opens > 0)
        {
            return -EMFILE;
        }

        node->read_opens++;
    }

    return insert_fd(fd);
}

int kmount(struct dir_struct *mountpoint, int major, int partition)
{
    struct gendisk_struct *disk;
    if ((disk = find_gendisk(major)) == NULL)
    {
        return -1;
    }


    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        struct filesystem_struct *pfs = supported_filesystems[i];
        if (pfs != NULL && (pfs->fs_probe(disk, partition) >= 0))
        {
            klog(KLOG_INFO, "kmount(): major=%d, start=0x%x, size=%S, type=%s",
                 major,
                 disk->part_list[partition].sector_offset * 512,
                 disk->part_list[partition].sector_count * 512,
                 pfs->name);

            return pfs->fs_mount(pfs, mountpoint, disk, partition);
        }
    }

    klog(KLOG_WARN, "kmount(): filesystem not supported");
    return -1;
}




