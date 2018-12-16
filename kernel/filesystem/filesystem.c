#include "filesystem.h"
#include "filesystem/ext2.h"
#include "memory/kheap.h"
#include "drivers/devices.h"
#include "log.h"

#define SUP_FS_COUNT 10
filesystem_t *supported_filesystems[SUP_FS_COUNT];


void init_filesystems()
{
    for (int i = 0; i < SUP_FS_COUNT; i++)
        supported_filesystems[i] = NULL;

    install_ext2();
}

int install_fs(filesystem_t *fs)
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

int kmount(struct dir_struct *mountpoint, int major, int partition)
{
    struct gendisk_struct *disk;
    if ((disk = find_device(major)) == NULL)
    {
        return -1;
    }


    for (int i = 0; i < SUP_FS_COUNT; i++)
    {
        filesystem_t *pfs = supported_filesystems[i];
        if (pfs != NULL && (pfs->fs_probe(disk, partition) >= 0))
        {
            klog(KLOG_INFO, "kmount(): major=%d, start=0x%x, size=%S, type=%s",
                 major,
                 disk->part_list[partition].sector_offset * 512,
                 disk->part_list[partition].sector_count * 512,
                 pfs->name);
            return pfs->fs_mount(mountpoint, disk, partition);
        }
    }

    klog(KLOG_WARN, "kmount(): filesystem not supported");
    return -1;
}




