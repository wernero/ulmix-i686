#ifndef PCI_H
#define PCI_H

#include <types.h>

/* all PCI devices: */
#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0x02
#define PCI_COMMAND     0x04
#define PCI_STATUS      0x06
#define PCI_REVISION    0x08
#define PCI_PROG_IF     0x09
#define PCI_SUBCLASS    0x0a
#define PCI_CLASS       0x0b
#define PCI_CACHE_LINE  0x0c
#define PCI_LATENCY     0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST        0x0f
#define PCI_BAR0        0x10
#define PCI_BAR1        0x14

/* if header type == 0 */
#define PCI_BAR2        0x18
#define PCI_BAR3        0x1c
#define PCI_BAR4        0x20
#define PCI_BAR5        0x24
#define PCI_CIS_PTR     0x28
#define PCI_SS_VENDOR   0x2c
#define PCI_SS_ID       0x2e
#define PCI_EXP_ROM     0x30
#define PCI_CAP_PTR     0x34
#define PCI_INTR_LINE   0x3c
#define PCI_INTR_PIN    0x3d
#define PCI_MIN_GRANT   0x3e
#define PCI_MAX_LATENC  0x3f

struct pcidev_struct
{
    uint8_t bus_no;
    uint8_t slot_no;
    uint8_t function_no;

    uint16_t device;
    uint16_t vendor;
};

void setup_pci();

// read configuration space
uint16_t pci_cfg_read16(const struct pcidev_struct *device, uint8_t offset);
uint32_t pci_cfg_read32(const struct pcidev_struct *device, uint8_t offset);

// check if device is present
int pci_is_present(const uint8_t bus, const uint8_t slot);

#endif // PCI_H
