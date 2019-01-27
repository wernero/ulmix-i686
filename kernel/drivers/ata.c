#include "ata.h"
#include "util/util.h"
#include "log.h"
#include "memory/kheap.h"
#include "devices/devices.h"
#include "interrupts.h"
#include "sched/task.h"
#include "sched/block.h"
#include "filesystem/fs_syscalls.h"

#include <devices/partitions.h>
#include <errno.h>

#define SECTOR_SIZE         512

#define PRIMARY_ATA         0x1f0
#define SECONDARY_ATA       0x170

#define ATA_ERROR           0
#define ATA_FOUND           1
#define ATA_NOT_PRESENT     2
#define ATA_UNKNOWN         3

const char *errors[] = { "error", "found ATA device", "device not present", "unknown device" };

typedef struct
{
    int id;
    int io_base;
    int drive_select;
    uint16_t data[256];
    int irq;
    mutex_t *mtx;
} ata_drive_t;

static int get_drive_addr(int drive, ata_drive_t *ata_drive);
static int identify(ata_drive_t *ata_drive);
static void waitBSY(ata_drive_t *drive);
static void waitDRQ(ata_drive_t *drive);

static int ata_open(struct file_struct *fd, int flags, union drv_union drv);
static ssize_t ata_read(struct file_struct *fd, char *buf, size_t count);
static ssize_t ata_write(struct file_struct *fd, char *buf, size_t count);
static ssize_t ata_seek(struct file_struct *fd, size_t offset, int whence);

ata_drive_t *drives[4];

void ata_init()
{
    ata_drive_t ata_drive;
    for (int i = 0; i < 4; i++)
    {
        get_drive_addr(i, &ata_drive);
        int status = identify(&ata_drive);
        klog(KLOG_DEBUG, "ATA identify #%d: status = %s", i, errors[status]);

        if (status == ATA_FOUND)
        {
            drives[i] = kmalloc(sizeof(ata_drive_t), 1, "ata_drive_t");
            *(drives[i]) = ata_drive;
            (drives[i])->mtx = mutex();

            struct fd_fops_struct fops =
            {
                .open = ata_open,       // not implemented yet
                .close = NULL,      //      -"-
                .read = ata_read,
                .write = ata_write,
                .seek = ata_seek,
                .ioctl = NULL
            };

            part_scan(register_bd(MAJOR_ATA0 + i, 0, fops, 0, 0, SECTOR_SIZE));
        }
    }
}

static int identify(ata_drive_t *ata_drive)
{
    uint8_t status;

    outb(ata_drive->io_base + 6, ata_drive->drive_select);    // drive select
    outb(ata_drive->io_base + 2, 0x00);  // sector count = 0
    outb(ata_drive->io_base + 3, 0x00);  // lba low = 0
    outb(ata_drive->io_base + 4, 0x00);  // lba mid = 0
    outb(ata_drive->io_base + 5, 0x00);  // lba high = 0
    outb(ata_drive->io_base + 7, 0xec);  // IDENTIFY command

    if (inb(ata_drive->io_base + 7) == 0)
    {
        return ATA_NOT_PRESENT;
    }

    // wait until status bit 7 clears (BSY)
    while (inb(ata_drive->io_base + 7) & 0x80);

    if (inb(ata_drive->io_base + 4) ||
            inb(ata_drive->io_base + 5))
    {
        return ATA_UNKNOWN;
    }

    while (((status = inb(ata_drive->io_base + 7)) & 0x09) == 0);
    if (status & 0x01)
    {
        return ATA_ERROR;
    }

    repinsw(ata_drive->io_base, ata_drive->data, 256);
    return ATA_FOUND;
}

static int ata_open(struct file_struct *fd, int flags, union drv_union drv)
{
    if (drv.bd->major < MAJOR_ATA0 || drv.bd->major > MAJOR_ATA3)
        return -EINVAL;

    fd->drv_struct = drives[drv.bd->major - MAJOR_ATA0];
    return SUCCESS;
}

static ssize_t ata_read(struct file_struct *fd, char *buf, size_t count)
{
    ata_drive_t *drive = ((ata_drive_t*)fd->drv_struct);

    count /= SECTOR_SIZE;
    size_t offset = fd->lock_offset + fd->seek_offset;
    fd->seek_offset += count;

    klog(KLOG_DEBUG, "ata%d: read: count=%d offset=%d", drive->id, count, fd->seek_offset);

    // https://wiki.osdev.org/ATA_PIO_Mode
    // +2 R/W - Sector Counter Register
    // +3 R/W - Sector Number Register
    // +4 R/W - Cylinder Low Register
    // +5 R/W - Cylinder High Register
    // +6 R/W - Drive / Head Register

    mutex_lock(drive->mtx);

    outb(drive->io_base + 6, drive->drive_select);                      // drive select
    outb(drive->io_base + 2, (count >> 8) & 0xff);                      // count high      // TODO >> 24??
    outb(drive->io_base + 3, (offset >> 24) & 0xff);        // byte 4
    outb(drive->io_base + 4, 0x00);
    outb(drive->io_base + 5, 0x00);
    outb(drive->io_base + 2, count & 0xff);                             // count low
    outb(drive->io_base + 3, offset & 0xff);              // byte 1
    outb(drive->io_base + 4, (offset >> 8) & 0xff);         // byte 2
    outb(drive->io_base + 5, (offset >> 16) & 0xff);        // byte 3

    waitBSY(drive);
    outb(drive->io_base + 7, 0x24);                                     // READ SECTORS EXT
    for (size_t i = 0; i < count; i++)
    {
        waitBSY(drive);
        waitDRQ(drive);

        repinsw(drive->io_base, (uint16_t*)(buf + SECTOR_SIZE * i), 256);
    }

    mutex_unlock(drive->mtx);
    return count;
}

static ssize_t ata_write(struct file_struct *fd, char *buf, size_t count)
{
    ata_drive_t *drive = ((ata_drive_t*)fd->drv_struct);
    klog(KLOG_DEBUG, "ata_write(): PIO mode: writing %d sectors to ATA #%d", count, drive->id);

    size_t offset = fd->lock_offset + fd->seek_offset;
    fd->seek_offset += count;

    mutex_lock(drive->mtx);

    outb(drive->io_base + 6, drive->drive_select);                      // drive select
    outb(drive->io_base + 2, (count >> 8) & 0xff);                      // count high     // >> 24 ???
    outb(drive->io_base + 3, (offset >> 24) & 0xff);        // byte 4
    outb(drive->io_base + 4, 0x00);
    outb(drive->io_base + 5, 0x00);
    outb(drive->io_base + 2, count & 0xff);                             // count low
    outb(drive->io_base + 3, (offset) & 0xff);              // byte 1
    outb(drive->io_base + 4, (offset >> 8) & 0xff);         // byte 2
    outb(drive->io_base + 5, (offset >> 16) & 0xff);        // byte 3

    waitBSY(drive);
    outb(drive->io_base + 7, 0x34);                                     // WRITE SECTORS EXT
    for (size_t i = 0; i < count; i++)
    {
        waitBSY(drive);
        waitDRQ(drive);

        outw(drive->io_base, buf[0]);
    }

    outb(drive->io_base + 7, 0xe7); // CACHE FLUSH
    waitBSY(drive);

    mutex_unlock(drive->mtx);
    return count;
}

static ssize_t ata_seek(struct file_struct *fd, size_t offset, int whence)
{
    if (offset % SECTOR_SIZE != 0)
        return -EINVAL;

    switch(whence)
    {
    case SEEK_SET:
        fd->seek_offset = offset / SECTOR_SIZE;
        break;
    case SEEK_CUR:
        fd->seek_offset += offset / SECTOR_SIZE;
        break;
    default:
        return -1;
    }

    return fd->seek_offset * SECTOR_SIZE;
}

/*static void waitRDY(ata_drive_t *drive)
{
    while (inb(drive->io_base + 7) & 0x40);
}*/

static void waitBSY(ata_drive_t *drive)
{
    while (inb(drive->io_base + 7) & 0x80);
}

static void waitDRQ(ata_drive_t *drive)
{
    while(!(inb(drive->io_base + 7) & 0x08));
}

static int get_drive_addr(int drive, ata_drive_t *ata_drive)
{
    ata_drive->id = drive;
    switch (drive)
    {
    case 0:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0x40; // 0x40
        ata_drive->irq = 0x2e;
        break;
    case 1:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0x50; // 0x50
        ata_drive->irq = 0x2e;
        break;
    case 2:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0x40; // 0x40
        ata_drive->irq = 0x2f;
        break;
    case 3:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0x50; // 0x50
        ata_drive->irq = 0x2f;
        break;
    default:
        return -1;
    }

    return 0;
}
