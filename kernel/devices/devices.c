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

struct gendisk_struct *register_bd(int major, int minor, struct fd_fops_struct fops, size_t capacity, size_t sector_offset, size_t io_size)
{
    struct gendisk_struct *bd = kmalloc(sizeof(struct gendisk_struct), 1, "gendisk_struct");
    bd->fops = fops;
    bd->major = major;
    bd->minor = minor;
    bd->capacity = capacity;
    bd->offset = sector_offset;
    bd->io_size = io_size;

    insert_gendisk(bd);
    klog(KLOG_DEBUG, "new blk device: %d/%d, offset=%d", major, minor, sector_offset);
    return bd;
}

int register_cd(int major, int minor, struct fd_fops_struct fops)
{
    struct chardev_struct *cd = kmalloc(sizeof(struct chardev_struct), 1, "chardev_struct");
    cd->fops = fops;
    cd->major = major;
    cd->minor = minor;

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

struct gendisk_struct *find_gendisk(int major, int minor)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (blk_devices[i] == NULL)
            continue;
        if (major == blk_devices[i]->major &&
                minor == blk_devices[i]->minor)
        {
            return blk_devices[i];
        }
    }

    return NULL;
}

struct chardev_struct *find_chardev(int major, int minor)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (char_devices[i] == NULL)
            continue;
        if (major == char_devices[i]->major &&
                minor == char_devices[i]->minor)
        {
            return char_devices[i];
        }
    }

    return NULL;
}
