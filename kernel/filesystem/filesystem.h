#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "filesystem/path.h"
#include "filesystem/vfscore.h"

typedef struct
{
    int bootable;           // bootable = 1
    int is_lba48;           // uses LBA48 addressing (for more than 2TB of addressable space)
    uint8_t system_id;      // Filesystem ID
    uint32_t start_sector;  // offset in sectors
    uint32_t sector_count;  // total size
} mbr_entry_t;

typedef struct
{
    int (*fs_probe)(file_t *device, mbr_entry_t part);
    int (*fs_mount)(fnode_t *mountpoint, file_t *device, mbr_entry_t part);
    fd_t *(*fs_open)(fnode_t *file);
    ssize_t (*fs_read)(fd_t *file, char *buf, size_t count);
    ssize_t (*fs_write)(fd_t *file, char *buf, size_t count);
    int (*fs_close)(fd_t *file);
    char *name;
} filesystem_t;

void init_filesystems(void);
int  install_fs(filesystem_t *fs);
int  disk_mount(fnode_t *mount_point, file_t *device);


#endif // FILESYSTEM_H
