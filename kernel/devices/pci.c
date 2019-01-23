#include "pci.h"
#include <util/util.h>
#include <kdebug.h>

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
    for (int bus = 0; bus < 16; bus++)
    {
        for (int dev = 0; dev < 16; dev++)
        {
            for (int fnc = 0; fnc < 16; fnc++)
            {
                if ((vendor = get_vendor(bus, dev, fnc)) != 0xffff)
                {
                    klog(KLOG_DEBUG, "%d:%d:%d >> vendor=%x, device=%x", bus, dev, fnc,
                         vendor, get_device(bus, dev, fnc));
                }
            }
        }
    }
}
