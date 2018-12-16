#ifndef DEVICES_H
#define DEVICES_H

#include "util/util.h"
#include "sched/sync.h"

#define MAJOR_ATA0  8
#define MAJOR_ATA1  9
#define MAJOR_ATA2  10
#define MAJOR_ATA3  11
#define MAJOR_AUDIO 12

struct fops_struct
{
    int (*open)(void *drv_struct); // probably add access modes (readonly, writeonly, readwrite)
    int (*release)(void *drv_struct);

    ssize_t (*write)(void *drv_struct, char *buf, size_t count);
    ssize_t (*read)(void *drv_struct, char *buf, size_t count);
    ssize_t (*seek)(void *drv_struct, size_t offset, int whence);
};

struct hd_struct
{
    char name[16];
    size_t sector_offset;
    size_t sector_count;
    uint8_t fs_type;    // 0x83 -> linux native: ext2, ext3, ext4, ReiserFS
};

struct gendisk_struct
{
    mutex_t *lock;
    char name[16];
    size_t capacity;
    int part_count;
    int major;
    void *drv_struct;
    struct hd_struct part_list[4]; // 4 partitions
    struct fops_struct fops;
};

void scan_devices(void);
struct gendisk_struct *find_device(int major);
int register_bd(int major, char *name, void *drv_struct, struct fops_struct fops, size_t capacity);

#endif // DEVICES_H
