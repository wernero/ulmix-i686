#include <pci.h>
#include <errno.h>
#include <debug.h>
#include <mem.h>

#define DEVICES 128
#define DRIVERS 128

static struct pcidev_struct *devices[DEVICES];
static const struct pci_driver_struct *drivers[DRIVERS];

static void add_pci_device(unsigned id, int bus, int slot, int fnc)
{
    struct pcidev_struct *device = kmalloc(sizeof(struct pcidev_struct), 1, "pcidev_struct");
    devices[id] = device;

    device->bus_no = bus;
    device->slot_no = slot;
    device->function_no = fnc;

    device->vendor_id = pci_cfg_read16(device, PCI_VENDOR_ID);
    device->device_id = pci_cfg_read16(device, PCI_DEVICE_ID);
    device->revision = pci_cfg_read8(device, PCI_REVISION);
    device->prog_if = pci_cfg_read8(device, PCI_PROG_IF);
    device->subclass = pci_cfg_read8(device, PCI_SUBCLASS);
    device->class = pci_cfg_read8(device, PCI_CLASS);

    kprintf("%02d:%02d:%d %04x:%04x class=%02x, subclass=%02x, prog_if=%02x\n",
            device->bus_no,
            device->slot_no,
            device->function_no,
            device->vendor_id,
            device->device_id,
            device->class,
            device->subclass,
            device->prog_if);

    for (int i = 0; i < DRIVERS; i++)
    {
        if (drivers[i] != NULL)
        {

            for (int j = 0; drivers[i]->ids[j].vendor_id != 0; j++)
            {
                if (drivers[i]->ids[j].vendor_id == device->vendor_id
                    && drivers[i]->ids[j].device_id == device->device_id)
                {
                    drivers[i]->driver_probe(device);
                }
            }
        }
    }
}

int pci_register_driver(const struct pci_driver_struct *drv)
{
    for (int i = 0; i < DRIVERS; i++)
    {
        if (drivers[i] == NULL)
        {
            drivers[i] = drv;
            return 0;
        }
    }
    return -EAGAIN;
}

void setup_pci()
{
    kprintf("\n === PCI SCAN ===\n");

    unsigned pci_device_index = 0;
    for (int bus = 0; bus < 256; bus++)
    {
        for (int slot = 0; slot < 32; slot++)
        {
            if (pci_is_present(bus, slot, 0))
            {
                for (int fnc = 0; fnc < 8; fnc++)
                {
                    if (pci_is_present(bus, slot, fnc))
                    {
                        add_pci_device(pci_device_index++, bus, slot, fnc);
                    }
                }
            }
        }
    }
}
