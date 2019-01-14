#include "devices.h"
#include "ata.h"
#include "keyboard.h"
#include "partitions.h"
#include <memory/kheap.h>
#include <kdebug.h>
#include <video/tty.h>

static void insert_gendisk(struct gendisk_struct *bd);
static void insert_chardev(struct chardev_struct *cd);
gendevice_t devices[20];
int device_index = 0;

void scan_devices()
{
    klog(KLOG_DEBUG, "scanning devices");

    ata_init();
    keyboard_setup();
    tty_setup();
}

struct gendisk_struct *find_gendisk(int major)
{
    for (int i = 0; i < device_index; i++)
    {
        if (major == devices[i].gendisk->major)
        {
            return devices[i].gendisk;
        }
    }

    return NULL;
}

struct chardev_struct *find_chardev(int major)
{
    for (int i = 0; i < device_index; i++)
    {
        if (major == devices[i].chardev->major)
        {
            return devices[i].chardev;
        }
    }

    return NULL;
}

int register_bd(int major, char *name, void *drv_struct, struct fops_struct fops, size_t capacity)
{
    struct gendisk_struct *bd = kmalloc(sizeof(struct gendisk_struct), 1, "gendisk_struct");
    strcpy(bd->name, name);
    bd->fops = fops;
    bd->lock = mutex();
    bd->major = major;
    bd->capacity = capacity;
    bd->part_count = 0;
    bd->drv_struct = drv_struct;

    if (part_scan(bd) < 0)
    {
        klog(KLOG_DEBUG, "register_bd(): %s: partition scan failed", name);
    }

    insert_gendisk(bd);
    return 0;
}

int register_cd(int major, char *name, struct fops_struct fops)
{
    struct chardev_struct *cd = kmalloc(sizeof(struct chardev_struct), 1, "chardev_struct");
    cd->fops = fops;
    cd->major = major;
    strcpy(cd->name, name);

    insert_chardev(cd);
    return 0;
}

static void insert_gendisk(struct gendisk_struct *bd)
{
    gendevice_t dev;
    dev.gendisk = bd;
    devices[device_index++] = dev;
}

static void insert_chardev(struct chardev_struct *cd)
{
    gendevice_t dev;
    dev.chardev = cd;
    devices[device_index++] = dev;
    klog(KLOG_DEBUG, "chardev #%d allocated", device_index-1);
}
