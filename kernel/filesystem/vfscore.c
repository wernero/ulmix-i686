#include "vfscore.h"
#include "filesystem/filesystem.h"
#include "log.h"

struct dir_struct root;

void vfs_init()
{
    root.mountpoint = 0;

    init_filesystems();

    if (kmount(&root, "ata0", 0) < 0) // -> ata0, partition 0
    {
        klog(KLOG_WARN, "unable to mount root fs"); // actually FAILURE!!!
    }
}
