#ifndef PCI_H
#define PCI_H

#include <util/util.h>

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

    pci_device_id_t id;
};

void setup_pci(void);

int pci_register(pci_device_id_t *idlist, struct pci_ops_struct pci_ops);

#endif // PCI_H
