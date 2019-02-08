/*
 * driver for the RTL 8139 ethernet chip
 * Copyright (C) 2019
 * Written by Alexander Ulmer
 */

#include <kdebug.h>
#include <devices/pci.h>
#include <memory/kheap.h>
#include <interrupts.h>
#include <inet/netdev.h>

#define DRVNAME "rtl8139"

enum rtl8139_registers
{
    tx_cmd0         = 0x10,
    tx_cmd1         = 0x14,
    tx_cmd2         = 0x18,
    tx_cmd3         = 0x1c,
    tx_buf0         = 0x20,
    tx_buf1         = 0x24,
    tx_buf2         = 0x28,
    tx_buf3         = 0x2c,
    rx_buf          = 0x30,
    intr_mask       = 0x3c,
    intr_status     = 0x3e,
    chip_cmd        = 0x37,
    packet_addr     = 0x38,
    buf_addr        = 0x3a,
    config1         = 0x52,
    rx_config       = 0x44
};

struct rtl8139dev
{
    uint32_t iobase;
    int interrupt;
    uint16_t packet_addr;
    unsigned char mac[6];
    unsigned long rx_bytes;
};

static pci_device_id_t idtable[] =
{
    { 0x10ec, 0x8139 },
    { 0x10ec, 0x8138 },
    { 0x1113, 0x1211 },
    { 0x1500, 0x1360 },
    { 0x4033, 0x1360 },
    { 0x1186, 0x1300 },
    { 0x1186, 0x1340 },
    { 0x13d1, 0xab06 },
    { 0x1259, 0xa117 },
    { 0x1259, 0xa11e },
    { 0x14ea, 0xab06 },
    { 0x14ea, 0xab07 },
    { 0x11db, 0x1234 },
    { 0x1432, 0x9130 },
    { 0x02ac, 0x1012 },
    { 0x018a, 0x0106 },
    { 0x126c, 0x1211 },
    { 0x1743, 0x8139 },
    { 0x021b, 0x8139 },

    { 0, 0 }
};

struct eth_pkg
{
    unsigned char dest_mac[6];
    unsigned char src_mac[6];
    uint16_t type;
};

struct rtl8139dev *info;
unsigned char rx_buffer[8192 + 16]; // receive buffer

static void rtl8139_intr(void)
{
    if (info == NULL)
        return;

    uint16_t read_end = inw(info->iobase + buf_addr);

    uint16_t packet_length;
    while (info->packet_addr < read_end)
    {
        packet_length = *((uint16_t*)(rx_buffer + info->packet_addr + 2));

        klog(KLOG_INFO, DRVNAME ": size=%d, dest=%M, src=%M",
             packet_length,
             (unsigned long)(rx_buffer + info->packet_addr + 4),
             (unsigned long)(rx_buffer + info->packet_addr + 10)
             );

        info->packet_addr += packet_length + 4;
        if (info->packet_addr % 4)
            info->packet_addr += 4 - (info->packet_addr % 4);

        //outw(info->iobase + packet_addr, info->packet_addr - 0x10);

    }

    outw(info->iobase + intr_status, 0x01);
}


ssize_t rtl8139_transmit(unsigned char *mesg, size_t count)
{
    hexdump(KLOG_INFO, mesg, count);
}

static int rtl8139_probe(pci_device_t *dev)
{
    klog(KLOG_INFO, "RTL8139 Fast Ethernet Controller");
    dev->drv_struct = kmalloc(sizeof(struct rtl8139dev), 1, "rtl8139 drv");
    info = (struct rtl8139dev*)dev->drv_struct;
    info->packet_addr = 0;
    info->iobase = pci_read32(dev, PCI_BAR0) & 0xfffffffe;
    info->interrupt = pci_read8(dev, PCI_INTR_LINE) + 32;
    info->rx_bytes = 0;
    for (int i = 0; i < 6; i++)
        info->mac[i] = inb(info->iobase + i);

    klog(KLOG_DEBUG, DRVNAME ": DMA: enable PCI bus mastering");
    uint16_t pci_command = pci_read16(dev, PCI_COMMAND);
    pci_command |= 0x04;
    pci_write16(dev, PCI_COMMAND, pci_command);

    klog(KLOG_DEBUG, DRVNAME ": power on, sw reset, hw_addr=%M", info->mac);
    outb(info->iobase + config1, 0x00);
    outb(info->iobase + chip_cmd, 0x10);
    while (inb(info->iobase + chip_cmd) & 0x10);

    klog(KLOG_DEBUG, DRVNAME ": RX DMA buffer is at 0x%x", (unsigned long)rx_buffer);
    outl(info->iobase + rx_buf, (uint32_t)rx_buffer);

    klog(KLOG_DEBUG, DRVNAME ": enable interrupt for TX_OK, RX_OK");
    irq_install_handler(info->interrupt, rtl8139_intr);
    outw(info->iobase + intr_mask, 0x0005);

    klog(KLOG_DEBUG, DRVNAME ": RX config set to promiscous");
    outl(info->iobase + rx_config, 0xf | (1 << 7));

    klog(KLOG_DEBUG, DRVNAME ": Enable RX and TX");
    outb(info->iobase + chip_cmd, 0x0c);

    // make the services available to the kernel
    struct netdev_struct netdev;
    netdev.name = "rtl8139";
    memcpy(&netdev.hw_addr, info->mac, 6);
    netdev.send = rtl8139_transmit;

    netdev_register(netdev);
    return 0;
}

int setup_rtl8139(void)
{
    struct pci_ops_struct pci_ops;
    pci_ops.probe = rtl8139_probe;
    pci_register(idtable, pci_ops);
    return SUCCESS;
}
