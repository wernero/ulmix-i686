#include "pci.h"
#include <util/util.h>
#include <kdebug.h>

#define MAX_PCI_DEVICES 256
static int pci_devices_index = 0;
static pci_device_t pci_devices[MAX_PCI_DEVICES];

static pci_device_t *find_device(pci_device_id_t dev_id)
{
    for (int i = 0; i < MAX_PCI_DEVICES; i++)
    {
        if (pci_devices[i].id.vendor == dev_id.vendor &&
                pci_devices[i].id.device == dev_id.device)
            return &(pci_devices[i]);
    }
    return NULL;
}

static void add_pci_device(uint8_t bus, uint8_t dev, uint8_t fnc, uint16_t vendor, uint16_t device)
{
    pci_devices[pci_devices_index].bus = bus;
    pci_devices[pci_devices_index].device = dev;
    pci_devices[pci_devices_index].function = fnc;
    pci_devices[pci_devices_index].id.vendor = vendor;
    pci_devices[pci_devices_index].id.device = device;
}

int pci_register(pci_device_id_t *idlist, struct pci_ops_struct pci_ops)
{
    pci_device_id_t *current_id;
    pci_device_t *device;
    while ((current_id = idlist++)->vendor != 0x0000)
    {
        if ((device = find_device(*current_id)) != NULL)
        {
            if (pci_ops.probe(device) == 0)
            {
                device->driver_attached = 1;
                device->driver_ops = pci_ops;
            }
        }
    }
    return SUCCESS;
}

static uint32_t pci_cfg_addr(pci_device_t *dev, int offset)
{
    uint32_t lbus  = (uint32_t)dev->bus;
    uint32_t lslot = (uint32_t)dev->device;
    uint32_t lfunc = (uint32_t)dev->function;

    return (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
}

uint32_t pci_read32(pci_device_t *dev, int offset)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    return inl(0xcfc);
}

void pci_write32(pci_device_t *dev, int offset, uint32_t val)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    outl(0xcfc, val);
}

uint16_t pci_read16(pci_device_t *dev, int offset)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    return (uint16_t)((inl(0xcfc) >> ((offset & 2) * 8)) & 0xffff);
}

void pci_write16(pci_device_t *dev, int offset, uint16_t val)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    outl(0xcfc, val << ((offset & 2) * 8));
}

uint8_t pci_read8(pci_device_t *dev, int offset)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    return (uint8_t)((inl(0xcfc) >> ((offset & 3) * 8)) & 0xff);
}

void pci_write8(pci_device_t *dev, int offset, uint8_t val)
{
    outl(0xcf8, pci_cfg_addr(dev, offset));
    outl(0xcfc, val << ((offset & 3) * 8));
}



static uint16_t pci_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;

    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    outl(0xcf8, address);
    return ((uint16_t)((inl(0xcfc) >> ((offset & 2) * 8)) & 0xffff));
}

static uint16_t get_vendor(uint8_t bus, uint8_t device, uint8_t function)
{
    return pci_read(bus, device, function, 0);
}

static uint16_t get_device(uint8_t bus, uint8_t device, uint8_t function)
{
    return pci_read(bus, device, function, 2);
}

void setup_pci()
{
    uint16_t vendor;
    for (int bus = 0; bus < 32; bus++)
    {
        for (int dev = 0; dev < 32; dev++)
        {
            for (int fnc = 0; fnc < 8; fnc++)
            {
                if ((vendor = get_vendor(bus, dev, fnc)) != 0xffff)
                {
                    uint16_t device = get_device(bus, dev, fnc);
                    klog(KLOG_DEBUG, "%d:%d:%d >> vendor=%x, device=%x", bus, dev, fnc,
                         vendor, device);
                    add_pci_device(bus, dev, fnc, vendor, device);
                }
            }
        }
    }
}
