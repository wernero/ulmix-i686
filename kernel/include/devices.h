#ifndef DEVICES_H
#define DEVICES_H

#include <types.h>
#include <fops.h>

struct chardev_struct
{
    unsigned int major;
    unsigned int minor;

    struct fops_struct fops;
};

struct hd_struct
{
    size_t sect_offset;
    size_t sect_count;

    unsigned char fs_type;
};

struct gendisk_struct
{
    // to be initialized by the device driver:
    unsigned int major;
    unsigned int minor;

    struct fops_struct fops;

    size_t io_size;
    size_t capacity;

    // initialized by partition table reader:
    size_t part_count;
    struct hd_struct part_list[4];
};

int register_chardev(struct chardev_struct *cd);
int register_blkdev(struct gendisk_struct *bd);

struct chardev_struct *get_chardev(unsigned int major, unsigned int minor);
struct gendisk_struct *get_blkdev(unsigned int major, unsigned int minor);

#endif // DEVICES_H
