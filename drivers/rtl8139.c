/*
 * driver for the RTL 8139 ethernet chip
 * Copyright (C) 2019
 * Written by Alexander Ulmer
 */

#include <kdebug.h>
#include <devices/pci.h>

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

static int rtl8139_probe(pci_device_t *dev)
{
    klog(KLOG_INFO, "RTL8139 Fast Ethernet driver");

    klog(KLOG_DEBUG, "probing %d:%d:%d", dev->bus, dev->device, dev->function);

    return 0;
}

int setup_rtl8139(void)
{
    struct pci_ops_struct pci_ops;
    pci_ops.probe = rtl8139_probe;
    pci_register(idtable, pci_ops);
    return SUCCESS;
}
