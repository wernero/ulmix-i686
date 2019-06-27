#include <pci.h>
#include <debug.h>

void setup_pci()
{
    kprintf("\n === PCI SCAN ===\n");
    for (int bus = 0; bus < 256; bus++)
    {
        for (int device = 0; device < 32; device++)
        {
            if (pci_is_present(bus, device, 0))
            {
                for (int fnc = 0; fnc < 8; fnc++)
                {
                    if (pci_is_present(bus, device, fnc))
                    {
                        kprintf("%02d:%02d:%d %s\n",
                                bus, device, fnc, "present");

                    }
                }
            }
        }
    }
}
