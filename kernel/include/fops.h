#ifndef FOPS_H
#define FOPS_H

#include <types.h>
#include <devices.h>

struct bd_fops_struct
{
    ssize_t (*read)(void *drv_struct, unsigned char *buffer, size_t blocks, size_t lba);
    ssize_t (*write)(void *drv_struct, unsigned char *buffer, size_t blocks, size_t lba);
};

struct cd_fops_struct
{
    ssize_t (*read)(void *drv_struct, unsigned char *buffer, size_t bytes, size_t offset);
    ssize_t (*write)(void *drv_struct, unsigned char *buffer, size_t bytes, size_t offset);
};

struct fs_fops_struct
{
    const char name;
    int (*fs_probe)(struct hd_struct *part);
};

#endif // FOPS_H
