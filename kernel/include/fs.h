#ifndef FS_H
#define FS_H

#include <fops.h>
#include <devices.h>

struct fs_struct
{
    const char *name;
    uint8_t mbr_id;

    struct fs_fops_struct fops;
};

int register_fs(const struct fs_struct *fs);
int probe_fs(struct hd_struct *part);
void gendisk_partscan(struct gendisk_struct *bd);

int do_mount(int dev_major, int dev_minor, const char *mount_path);

#endif // FS_H
