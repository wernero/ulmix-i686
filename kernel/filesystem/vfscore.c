#include "vfscore.h"
#include "filesystem/filesystem.h"
#include "log.h"

void vfs_init()
{
    // init VFS

    init_filesystems();

    if (kmount("/", "ata0", 0) < 0) // -> ata0, partition 0
    {
        klog(KLOG_FAILURE, "unable to mount root fs");
    }
}
