#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "filesystem/vfscore.h"
#include "filesystem/partitions.h"

typedef struct _mbr_entry mbr_entry_t;
typedef struct
{
    int (*fs_probe)(fd_t *device);
    int (*fs_mount)(fnode_t *mountpoint, fd_t *device);
    fd_t *(*fs_open)(fnode_t *file);
    ssize_t (*fs_read)(fd_t *file, char *buf, size_t count);
    ssize_t (*fs_write)(fd_t *file, char *buf, size_t count);
    int (*fs_close)(fd_t *file);
    char *name;
} filesystem_t;

void init_filesystems(void);
int  install_fs(filesystem_t *fs);
int part_mount(fnode_t *mount_point, fd_t *device, mbr_entry_t partition);


#endif // FILESYSTEM_H
