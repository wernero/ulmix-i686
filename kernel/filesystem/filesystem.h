#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "filesystem/vfscore.h"
#include "drivers/devices.h"

typedef struct
{
    int (*fs_probe)(struct gendisk_struct *bd, int part);
    int (*fs_mount)(struct dir_struct *mountpoint, struct gendisk_struct *bd, int part);
    int (*fs_get_direntry)(struct dir_struct *miss);
    int (*fs_get_inode)(struct dir_struct *parent, unsigned long inode_no);
    char *name;
} filesystem_t;

void init_filesystems(void);
int  install_fs(filesystem_t *fs);
int kmount(struct dir_struct *mountpoint, char *device, int partition);


#endif // FILESYSTEM_H
