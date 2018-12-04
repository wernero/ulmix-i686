#include "ata.h"
#include "util/util.h"
#include "log.h"
#include "interrupts.h"

#define PRIMARY_ATA         0x1f0
#define PRIM_SECTOR_COUNT   0x1f2
#define PRIM_LBA_LOW        0x1f3
#define PRIM_LBA_MID        0x1f4
#define PRIM_LBA_HIGH       0x1f5
#define PRIM_DRIVE_SELCT    0x1f6
#define PRIM_CMD_STAT       0x1f7

#define SECONDARY_ATA       0x170
#define SEC_SECTOR_COUNT    0x172
#define SEC_LBA_LOW         0x173
#define SEC_LBA_MID         0x174
#define SEC_LBA_HIGH        0x175
#define SEC_DRIVE_SELCT     0x176
#define SEC_CMD_STAT        0x177

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
} ata_drive_t;

static int get_drive_addr(int drive, ata_drive_t *ata_drive);
static int identifiy(ata_drive_t *ata_drive);

static void irq_primary(void);
static void irq_secondary(void);

void ata_init()
{
    irq_install_handler(0x2e, irq_primary);
    irq_install_handler(0x2f, irq_secondary);

    ata_drive_t ata_drive;
    for (int i = 0; i < 4; i++)
    {
        get_drive_addr(i, &ata_drive);
        int status = identifiy(&ata_drive);
        klog(KLOG_DEBUG, "ATA identify #%d: status = %s", i, errors[status]);

        if (status == ATA_FOUND)
        {
            // register device
        }
    }
}

static int identifiy(ata_drive_t *ata_drive)
{
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

    uint8_t status;
    while (((status = inb(ata_drive->io_base + 7)) & 0x09) == 0);

    if (status & 0x01)
    {
        return ATA_ERROR;
    }

    repinsw(ata_drive->io_base, ata_drive->data, 256);
    return ATA_FOUND;
}

static void irq_primary(void)
{

}

static void irq_secondary(void)
{

}

static int get_drive_addr(int drive, ata_drive_t *ata_drive)
{
    ata_drive->id = drive;
    switch (drive)
    {
    case 0:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0xa0;
        break;
    case 1:
        ata_drive->io_base = PRIMARY_ATA;
        ata_drive->drive_select = 0xb0;
        break;
    case 2:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0xa0;
        break;
    case 3:
        ata_drive->io_base = SECONDARY_ATA;
        ata_drive->drive_select = 0xb0;
        break;
    default:
        return -1;
    }

    return 0;
}
