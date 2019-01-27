#ifndef DEVICES_H
#define DEVICES_H

#include <util/util.h>
#include <sched/sync.h>
#include <filesystem/fs_syscalls.h>

#define MAX_DEVICES     64

#define MAJOR_TTY       1
#define MAJOR_KEYBOARD  2
#define MAJOR_MOUSE     3
#define MAJOR_ATA0      8
#define MAJOR_ATA1      9
#define MAJOR_ATA2      10
#define MAJOR_ATA3      11
#define MAJOR_AUDIO     12

struct hd_struct
{
    char name[16];
    size_t sector_offset;
    size_t sector_count;
    uint8_t fs_type;    // 0x83 -> linux native: ext2, ext3, ext4, ReiserFS
};

struct gendisk_struct
{
    int major;
    int minor;
    size_t io_size;     // Block size
    size_t capacity;
    size_t offset;
    struct fd_fops_struct fops;

    int part_count;
    void *drv_struct;
    struct hd_struct part_list[4]; // 4 partitions
};

struct chardev_struct
{
    int major;
    int minor;
    struct fd_fops_struct fops;
};

void scan_devices(void);
struct gendisk_struct *find_gendisk(int major, int minor);
struct chardev_struct *find_chardev(int major, int minor);
struct gendisk_struct *register_bd(int major, int minor, struct fd_fops_struct fops, size_t capacity, size_t sector_offset, size_t io_size);
int register_cd(int major, int minor, struct fd_fops_struct fops);

#endif // DEVICES_H
