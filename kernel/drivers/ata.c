#include "ata.h"
#include "util/util.h"
#include "log.h"
#include "memory/kheap.h"
#include "drivers/devices.h"
#include "interrupts.h"
#include "sched/task.h"
#include "sched/block.h"
#include "filesystem/fs_syscalls.h"

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
    size_t seek_offset;
    uint16_t data[256];
    int irq;
} ata_drive_t;

static int get_drive_addr(int drive, ata_drive_t *ata_drive);
static void get_name(ata_drive_t *drive, char *buf);
static int identify(ata_drive_t *ata_drive);
static void waitBSY(ata_drive_t *drive);
static void waitDRQ(ata_drive_t *drive);

static ssize_t ata_read(void *dev_struct, char *buf, size_t count);
static ssize_t ata_write(void *dev_struct, char *buf, size_t count);
static ssize_t ata_seek(void *dev_struct, size_t offset, int whence);

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
            ata_drive_t *drive = kmalloc(sizeof(ata_drive_t), 1, "ata_drive_t");
            *drive = ata_drive;

            char drive_name[16];
            get_name(&ata_drive, drive_name);

            struct fops_struct fops =
            {
                .open = NULL,       // not implemented yet
                .release = NULL,    //      -"-
                .read = ata_read,
                .write = ata_write,
                .seek = ata_seek
            };

            register_bd(drive_name, (void*)drive, fops, 0);
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
    ata_drive->seek_offset = 0;
    return ATA_FOUND;
}

static ssize_t ata_read(void *dev_struct, char *buf, size_t count)
{
    ata_drive_t *drive = ((ata_drive_t*)dev_struct);
    klog(KLOG_DEBUG, "ata_read(): read count=%d offset=%d from ATA #%d", count, drive->seek_offset, drive->id);

    outb(drive->io_base + 6, drive->drive_select);  // drive select
    outb(drive->io_base + 2, (count >> 8) & 0xff);  // count high
    outb(drive->io_base + 3, (drive->seek_offset >> 6) & 0xff);
    outb(drive->io_base + 4, 0x00);
    outb(drive->io_base + 3, 0x00);
    outb(drive->io_base + 2, count & 0xff);         // count low
    outb(drive->io_base + 3, (drive->seek_offset) & 0xff);
    outb(drive->io_base + 4, (drive->seek_offset >> 2) & 0xff);
    outb(drive->io_base + 5, (drive->seek_offset >> 4) & 0xff);

    waitBSY(drive);
    outb(drive->io_base + 7, 0x24); // READ SECTORS EXT
    for (size_t i = 0; i < count; i++)
    {
        waitBSY(drive);
        waitDRQ(drive);

        repinsw(drive->io_base, (uint16_t*)(buf + SECTOR_SIZE * i), 256);
    }
    return count;
}

static ssize_t ata_write(void *dev_struct, char *buf, size_t count)
{
    ata_drive_t *drive = ((ata_drive_t*)dev_struct);
    klog(KLOG_DEBUG, "ata_read(): PIO mode: writing %d sectors to ATA #%d", count, drive->id);

    outb(drive->io_base + 6, drive->drive_select);  // drive select
    outb(drive->io_base + 2, (count >> 8) & 0xff);  // count high
    outb(drive->io_base + 3, (drive->seek_offset >> 6) & 0xff);
    outb(drive->io_base + 4, 0x00);
    outb(drive->io_base + 3, 0x00);
    outb(drive->io_base + 2, count & 0xff);         // count low
    outb(drive->io_base + 3, (drive->seek_offset) & 0xff);
    outb(drive->io_base + 4, (drive->seek_offset >> 2) & 0xff);
    outb(drive->io_base + 5, (drive->seek_offset >> 4) & 0xff);

    waitBSY(drive);
    outb(drive->io_base + 7, 0x34); // WRITE SECTORS EXT
    for (size_t i = 0; i < count; i++)
    {
        waitBSY(drive);
        waitDRQ(drive);

        outw(drive->io_base, buf[0]);
    }

    outb(drive->io_base + 7, 0xe7); // CACHE FLUSH
    waitBSY(drive);
    return count;
}

static ssize_t ata_seek(void *dev_struct, size_t offset, int whence)
{
    ata_drive_t *drive = ((ata_drive_t*)dev_struct);

    switch(whence)
    {
    case SEEK_SET:
        drive->seek_offset = offset;
        break;
    case SEEK_CUR:
        drive->seek_offset += offset;
        break;
    default:
        return -1;
    }

    return drive->seek_offset;
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

static void get_name(ata_drive_t *drive, char *buf)
{
    strcpy(buf, "ata");
    int len = strlen(buf);
    buf[len] = '0' + drive->id;
    buf[len + 1] = 0;
}

static int get_drive_addr(int drive, ata_drive_t *ata_drive)
{
    ata_drive->id = drive;
    switch (drive)
    {
    case 0:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0x40;
        ata_drive->irq = 0x2e;
        break;
    case 1:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0x50;
        ata_drive->irq = 0x2e;
        break;
    case 2:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0x40;
        ata_drive->irq = 0x2f;
        break;
    case 3:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0x50;
        ata_drive->irq = 0x2f;
        break;
    default:
        return -1;
    }

    return 0;
}
