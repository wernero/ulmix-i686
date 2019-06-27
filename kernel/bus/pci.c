#include <pci.h>
#include <debug.h>

static struct pcidev_struct devices[32];

static void add_pci_device(unsigned id, int bus, int slot, int fnc)
{
    struct pcidev_struct *device = devices + id;
    device->bus_no = bus;
    device->slot_no = slot;
    device->function_no = fnc;

    device->vendor_id = pci_cfg_read16(device, PCI_VENDOR_ID);
    device->device_id = pci_cfg_read16(device, PCI_DEVICE_ID);
    device->revision = pci_cfg_read16(device, PCI_REVISION);
    device->prog_if = pci_cfg_read16(device, PCI_PROG_IF);
    device->subclass = pci_cfg_read16(device, PCI_SUBCLASS);
    device->class = pci_cfg_read16(device, PCI_CLASS);

    kprintf("%02d:%02d:%d %04x:%04x Class=%d, Subclass=%d (revision %02d)\n",
            device->bus_no,
            device->slot_no,
            device->function_no,
            device->vendor_id,
            device->device_id,
            device->class,
            device->subclass,
            device->revision);
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
