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

#define PRIMARY_BASE    0x1f0
#define PRIMARY_CTRL    0x3f6
#define SECONDARY_BASE  0x170
#define SECONDARY_CTRL  0x376

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
#define ATA_LIBAHI  0x05    // R/W: LBA high               16 bit
#define ATA_DR_HD   0x06    // R/W: drive and head select  8 bit
#define ATA_STATUS  0x07    // R:   status                 8 bit
#define ATA_COMMND  0x07    // W:   command                8 bit

#define ATA_ALT_ST  0x00    // R:   status duplicate (noirq)    8 bit
#define ATA_DRV_ADR 0x01    // R:   drive and head select       8 bit
#define ATA_DEVCTRL 0x00    // W:   reset bus, en/disable irq   8 bit

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

struct ata_dev_struct
{
    struct pcidev_struct pci;
    unsigned type;
    unsigned irq_line;
    unsigned base_io;
    unsigned ctrl_io;
};

static int ata_pci_probe(struct pcidev_struct *dev)
{
    if (dev->class != 1 && dev->subclass != 1)
        return -ENOSYS;

    struct ata_dev_struct *ata_dev = kmalloc(sizeof(struct ata_dev_struct), 1, "ata_dev_struct");


    kprintf("ATA controller got the device\n");

    return -ENOTSUP;
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
