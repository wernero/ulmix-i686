#include "devices.h"
#include "drivers/ata.h"
#include "drivers/partitions.h"
#include "memory/kheap.h"
#include "log.h"

static void insert_device(struct gendisk_struct *bd);
struct gendisk_struct *devices[20];
int device_index = 0;

void scan_devices()
{
    klog(KLOG_DEBUG, "scanning devices");

    ata_init();
    // ... more devices
}

struct gendisk_struct *find_device(int major)
{
    for (int i = 0; i < device_index; i++)
    {
        if (major == devices[i]->major)
        {
            return devices[i];
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

    insert_device(bd);
    return 0;
}

static void insert_device(struct gendisk_struct *bd)
{
    devices[device_index++] = bd;
}
