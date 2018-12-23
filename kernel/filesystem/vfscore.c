#include "vfscore.h"
#include "filesystem/filesystem.h"
#include "log.h"

struct dir_struct root;

void vfs_init()
{
	char mount_name[1] = "/";
    root.mountpoint = 1;
    
    memcpy(root.name, mount_name, 1);

    init_filesystems();

    if (kmount(&root, MAJOR_ATA0, 0) < 0) // -> ata0, partition 0
    {
        klog(KLOG_WARN, "unable to mount root fs"); // actually FAILURE!!!
    }
}
