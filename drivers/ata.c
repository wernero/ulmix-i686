/*
 * ULMIX ATA Hard Disk Driver
 * Copyright (C) 2018-2019
 * Written by Alexander Ulmer
 */

#include <pci.h>
#include <errno.h>
#include <types.h>
#include <debug.h>
#include <mem.h>
#include <asm.h>

#define PRIMARY1_BASE    0x1f0
#define PRIMARY1_CTRL    0x3f6
#define SECONDARY1_BASE  0x170
#define SECONDARY1_CTRL  0x376

#define ATA_PRIM_MASTER 1
#define ATA_PRIM_SLAVE  2
#define ATA_SEC_MASTER  3
#define ATA_SEC_SLAVE   4

#define ATA_DATA    0x00    // R/W: data register          16 bit
#define ATA_ERROR   0x01    // R:   last command error     16 bit
#define ATA_FEAT    0x01    // W:   feature control        16 bit
#define ATA_SCOUNT  0x02    // R/W: sector count           16 bit
#define ATA_LBALO   0x03    // R/W: LBA low                16 bit
#define ATA_LBAMID  0x04    // R/W: LBA mid                16 bit
#define ATA_LBAHI   0x05    // R/W: LBA high               16 bit
#define ATA_DR_HD   0x06    // R/W: drive and head select  8 bit
#define ATA_STATUS  0x07    // R:   status                 8 bit
#define ATA_COMMND  0x07    // W:   command                8 bit

#define ATA_ALT_ST  0x00    // R:   status duplicate (noirq)    8 bit
#define ATA_DRV_ADR 0x01    // R:   drive and head select       8 bit
#define ATA_DEVCTRL 0x00    // W:   reset bus, en/disable irq   8 bit

#define CMD_IDENTIFY    0xec

enum ata_errors
{
    AMNF    = BIT(0),   // address mark not found
    TKZNF   = BIT(1),   // track zero not found
    ABRT    = BIT(2),   // aborted
    MCR     = BIT(3),   // media change request
    IDNF    = BIT(4),   // id not found
    MC      = BIT(5),   // media changed
    UNC     = BIT(6),   // uncorrectable data error
    BBK     = BIT(7)    // bad block detected
};

enum ata_status
{
    ERROR   = BIT(0),   // error condition
    INDEX   = BIT(1),   // always zero
    CORR    = BIT(2),   // always zero
    DRQ     = BIT(3),   // ready to accept PIO data
    SRV     = BIT(4),   // overlapped mode service request
    DF      = BIT(5),   // drive fault error
    RDY     = BIT(6),   // drive is spun down
    BSY     = BIT(7)    // drive is preparing to send/receive
};

enum ata_types
{
    ATA_PRIMARY_MASTER      = 0,
    ATA_PRIMARY_SLAVE       = 1,
    ATA_SECONDARY_MASTER    = 2,
    ATA_SECONDARY_SLAVE     = 3
};

struct ata_dev_struct
{
    struct pcidev_struct *pci;
    enum ata_types type;
    unsigned base_io;
    unsigned ctrl_io;
    unsigned short data[256];
};

static int ata_identify(struct ata_dev_struct *dev)
{
    outb(dev->base_io + ATA_DR_HD, (dev->type % 2 == 0) ? 0xa0 : 0xb0);
    outb(dev->base_io + ATA_SCOUNT, 0x0);
    outb(dev->base_io + ATA_LBALO,  0x0);
    outb(dev->base_io + ATA_LBAMID, 0x0);
    outb(dev->base_io + ATA_LBAHI,  0x0);
    outb(dev->base_io + ATA_COMMND, CMD_IDENTIFY);

    if (inb(dev->base_io + ATA_STATUS) == 0x0)
        return 0;

    // poll until device becomes ready
    while ((inb(dev->base_io + ATA_STATUS) & BSY));

    if (inb(dev->base_io + ATA_LBAMID) != 0
        || inb(dev->base_io + ATA_LBAHI) != 0)
        return 0;

    while ((inb(dev->base_io + ATA_STATUS) & (DRQ | ERROR)) == 0);

    if ((inb(dev->base_io + ATA_STATUS) & ERROR) == 0)
    {
        repinsw(dev->base_io + ATA_DATA, dev->data, 256);
        return 1;
    }

    return 0;
}

static int get_ports(unsigned controller, enum ata_types type, unsigned *base_io, unsigned *ctrl_io)
{
    if (controller != 0)
        return -ENOSYS;

    if (type < 2)
    {
        *base_io = PRIMARY1_BASE;
        *ctrl_io = PRIMARY1_CTRL;
    }
    else
    {
        *base_io = SECONDARY1_BASE;
        *ctrl_io = SECONDARY1_CTRL;
    }

    return 0;
}

static int ata_pci_probe(struct pcidev_struct *dev)
{
    if (dev->class != 1 && dev->subclass != 1)
        return -ENOSYS;

    struct ata_dev_struct ata_dev;
    for (int i = 0; i < 4; i++)
    {
        ata_dev.type = i;
        if (get_ports(0, ata_dev.type, &ata_dev.base_io, &ata_dev.ctrl_io) < 0)
            return -EIO;
        ata_dev.pci = dev;

        if (ata_identify(&ata_dev))
        {
            kprintf("ata%d: disk attached\n", i);
            struct ata_dev_struct *drv_struct =
                    kmalloc(sizeof(struct ata_dev_struct), 1, "ata_dev_struct");
            *drv_struct = ata_dev;

            // register device file

            continue;
        }

        kprintf("ata%d: medium is not present or not ATA\n", i);
    }

    return 0;
}

static const struct pci_idpair_struct ata_pcidevices[] = {
    // { 0x8086, 0x7111 }, // Intel 82371AB/EB/MB PIIX4 IDE Controller
    { 0x8086, 0x7010 }, // Intel 82371SB PIIX3 IDE Controller [Natoma/Triton II]
    { NULL, NULL }
};

static const struct pci_driver_struct ata_driver = {
    ata_pcidevices, ata_pci_probe
};

void __init init_ata()
{
    pci_register_driver(&ata_driver);
}
