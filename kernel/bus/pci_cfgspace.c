#include <pci.h>
#include <asm.h>

#define CONFIG_ADDRESS  0xcf8
#define CONFIG_DATA     0xcfc

uint32_t pci_cfg_read32(const struct pcidev_struct *device, uint8_t offset)
{
    uint32_t address;
    uint32_t bus = device->bus_no;
    uint32_t slot = device->slot_no;
    uint32_t fnc = device->function_no;

    address = (bus << 16)
            | (slot << 11)
            | (fnc << 8)
            | (offset & 0xfc)
            | BIT(31);

    outl(CONFIG_ADDRESS, address);
    return inl(CONFIG_DATA);
}

uint16_t pci_cfg_read16(const struct pcidev_struct *device, uint8_t offset)
{
    uint32_t data = pci_cfg_read32(device, offset);
    return ((data >> ((offset & 2) * 8)) & 0xffff);
}

int pci_is_present(uint8_t bus, uint8_t slot, uint8_t fnc)
{
    const struct pcidev_struct dev = {
        .bus_no = bus,
        .slot_no = slot,
        .function_no = fnc
    };

    if (pci_cfg_read16(&dev, 0x00) == 0xffff)
        return 0;
    return 1;
}
