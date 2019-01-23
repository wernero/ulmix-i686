#include "devices.h"
#include "pci.h"
#include "partitions.h"
#include <drivers/ata.h>
#include <drivers/keyboard.h>
#include <memory/kheap.h>
#include <kdebug.h>
#include <video/tty.h>

static void insert_gendisk(struct gendisk_struct *bd);
static void insert_chardev(struct chardev_struct *cd);

struct gendisk_struct *blk_devices[MAX_DEVICES];
struct chardev_struct *char_devices[MAX_DEVICES];

void scan_devices()
{
    klog(KLOG_DEBUG, "scanning devices");

    setup_pci();
    ata_init();
    keyboard_setup();
    tty_setup();
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

int register_cd(int major, char *name, struct fd_fops_struct fops)
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
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (blk_devices[i] == NULL)
        {
            blk_devices[i] = bd;
            return;
        }
    }
}

static void insert_chardev(struct chardev_struct *cd)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (char_devices[i] == NULL)
        {
            char_devices[i] = cd;
            return;
        }
    }
}

struct gendisk_struct *find_gendisk(int major)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (blk_devices[i] == NULL)
            continue;
        if (major == blk_devices[i]->major)
        {
            return blk_devices[i];
        }
    }

    return NULL;
}

struct chardev_struct *find_chardev(int major)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (char_devices[i] == NULL)
            continue;
        if (major == char_devices[i]->major)
        {
            return char_devices[i];
        }
    }

    return NULL;
}
