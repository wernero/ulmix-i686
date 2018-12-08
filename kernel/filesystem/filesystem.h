#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "drivers/devices.h"

typedef struct
{
    int (*fs_probe)(struct gendisk_struct *bd, int part);
    int (*fs_mount)(const char *mountpoint, struct gendisk_struct *bd, int part);
    char *name;
} filesystem_t;

void init_filesystems(void);
int  install_fs(filesystem_t *fs);
int kmount(const char *mountpoint, char *device, int partition);


#endif // FILESYSTEM_H
