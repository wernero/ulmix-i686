#ifndef PCI_H
#define PCI_H

#include <util/util.h>

#define PCI_VENDOR      0x00    // 2
#define PCI_DEVICE      0x02    // 2
#define PCI_COMMAND     0x04    // 2
#define PCI_STATUS      0x06    // 2
#define PCI_REVISION    0x08    // 1
#define PCI_IF          0x09    // 1
#define PCI_SUBCLASS    0x0a    // 1
#define PCI_CLASS       0x0b    // 1
#define PIC_CACHE       0x0c    // 1
#define PCI_LATENCY     0x0d    // 1
#define PCI_HEADER      0x0e    // 1
#define PCI_BIST        0x0f    // 1
#define PCI_BAR0        0x10    // 4
#define PCI_BAR1        0x14    // 4
#define PCI_BAR2        0x18    // 4
#define PCI_BAR3        0x1c    // 4
#define PCI_BAR4        0x20    // 4
#define PCI_BAR5        0x24    // 4
#define PCI_CARD_CIS    0x28    // 4
#define PCI_SUBVENDOR   0x2c    // 2
#define PCI_SUBSYS      0x2e    // 2
#define PCI_EXP_ROM     0x30    // 4
#define PCI_CAP         0x34    // 1
#define PCI_INTR_LINE   0x3c    // 1
#define PCI_INTR_PIN    0x3d    // 1
#define PCI_MIN_GRANT   0x3e    // 1
#define PIC_MAX_LATENCY 0x3f    // 1

typedef struct _pci_device_struct pci_device_t;
struct pci_ops_struct
{
    int (*probe)(pci_device_t *dev);
};

typedef struct
{
    uint16_t vendor;
    uint16_t device;
} pci_device_id_t;

struct _pci_device_struct
{
    uint8_t bus;
    uint8_t device;
    uint8_t function;

    int driver_attached;
    struct pci_ops_struct driver_ops;
    void *drv_struct;

    pci_device_id_t id;
};

void setup_pci(void);

int pci_register(pci_device_id_t *idlist, struct pci_ops_struct pci_ops);

uint32_t pci_read32(pci_device_t *dev, int offset);
void pci_write32(pci_device_t *dev, int offset, uint32_t val);
uint16_t pci_read16(pci_device_t *dev, int offset);
void pci_write16(pci_device_t *dev, int offset, uint16_t val);
uint8_t pci_read8(pci_device_t *dev, int offset);
void pci_write8(pci_device_t *dev, int offset, uint8_t val);


#endif // PCI_H
