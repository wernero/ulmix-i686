#include "filesystem.h"
#include "ext2.h"
#include "open.h"
#include <memory/kheap.h>
#include <devices/devices.h>
#include <kdebug.h>
#include <errno.h>

#define SUP_FS_COUNT 10
struct filesystem_struct *supported_filesystems[SUP_FS_COUNT];

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
    return file->parent->sb->fs->fs_get_inode(file);  /// get_inode function changed !!!!
}

int direntry_get_dir(struct dir_struct *dir)
{
    return dir->sb->fs->fs_get_direntry(dir);
}

int kmount(struct dir_struct *mountpoint, int major, int minor)
{
    struct file_struct *fd = kopen_device(BLOCKDEVICE, major, minor, O_RDWR);
    if (fd == NULL)
        return -ENODEV;

    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        struct filesystem_struct *pfs = supported_filesystems[i];
        if (pfs != NULL && (pfs->fs_probe(fd) >= 0))
        {
            klog(KLOG_INFO, "kmount(): major=%d, start=0x%x, size=%S, type=%s",
                 major,
                 fd->drv.bd->part_list[minor].sector_offset * 512,
                 fd->drv.bd->part_list[minor].sector_count * 512,
                 pfs->name);

            return pfs->fs_mount(pfs, mountpoint, fd);
        }
    }

    klog(KLOG_WARN, "kmount(): filesystem not supported");
    // kclose(fd);
    return -1;
}




