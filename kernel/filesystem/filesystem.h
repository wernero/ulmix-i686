#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "vfscore.h"
#include <util/util.h>
#include <devices/devices.h>

void init_filesystems(void);
int  install_fs(struct filesystem_struct *fs);
int direntry_get_dir(struct dir_struct *dir);
int kmount(struct dir_struct *mountpoint, int major, int minor);


#endif // FILESYSTEM_H
