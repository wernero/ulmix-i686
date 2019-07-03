#ifndef DEVICES_H
#define DEVICES_H

#include <types.h>
#include <fops.h>

#define MAJOR_ATA0  4
#define MAJOR_ATA1  5
#define MAJOR_ATA2  6
#define MAJOR_ATA3  7
#define MAJOR_ATA4  8
#define MAJOR_ATA5  9
#define MAJOR_ATA6  10
#define MAJOR_ATA7  11

struct chardev_struct
{
    unsigned int major;
    unsigned int minor;

    struct cd_fops_struct fops;
};

struct hd_struct
{
    size_t sect_offset;
    size_t sect_count;

    unsigned char fs_type;

    struct gendisk_struct *bd;
};

struct gendisk_struct
{
    // to be initialized by the device driver:
    unsigned int major;

    struct bd_fops_struct fops;

    size_t io_size;
    size_t capacity;

    // initialized by partition table reader:
    size_t part_count;
    struct hd_struct part_list[4];

    void *drv_struct;
};

int register_chardev(struct chardev_struct *cd);
int register_blkdev(struct gendisk_struct *bd);

struct chardev_struct *get_chardev(unsigned int major, unsigned int minor);
struct gendisk_struct *get_gendisk(unsigned int major); // minors are part of gendisk_struct
struct hd_struct *get_hdstruct(unsigned int major, unsigned int minor);

#endif // DEVICES_H
