#include "vfscore.h"
#include "filesystem.h"
#include <kdebug.h>

struct dir_struct root;

void vfs_init()
{
    root.mountpoint = &root;
    root.mnt_info = NULL;

    init_filesystems();

    if (kmount(&root, MAJOR_ATA0, 1) < 0) // -> ata0, partition 1 -> this is temporary
    {
        klog(KLOG_PANIC, "unable to mount root fs");
    }
}
