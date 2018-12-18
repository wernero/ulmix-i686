#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "filesystem/vfscore.h"
#include "drivers/devices.h"

void init_filesystems(void);
int  install_fs(struct filesystem_struct *fs);
int direntry_get_inode(struct direntry_struct *file);
int kmount(struct dir_struct *mountpoint, int major, int partition);


#endif // FILESYSTEM_H
