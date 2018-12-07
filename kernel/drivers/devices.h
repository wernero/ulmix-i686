#ifndef DEVICES_H
#define DEVICES_H

#include "util/util.h"
#include "filesystem/filesystem.h"

typedef enum
{
    DEV_FLOPPY,
    DEV_ATA_HDD,
    DEV_PARTITION
} device_type_t;

typedef struct
{
    void *drv_struct;
    ssize_t (*read) (void* drv_struct, char *buf, size_t count);
    ssize_t (*write)(void* drv_struct, char *buf, size_t count);
    ssize_t (*seek) (void* drv_struct, size_t offset, int whence);
} dev_t;

void scan_devices(void);
void register_device(device_type_t type,
                     void *drv_struct,
                     size_t io_size,
                     ssize_t (*read) (void* drv_struct, char *buf, size_t count),
                     ssize_t (*write)(void* drv_struct, char *buf, size_t count),
                     ssize_t (*seek) (void* drv_struct, size_t offset, int whence),
                     char *dev_name);

#endif // DEVICES_H
