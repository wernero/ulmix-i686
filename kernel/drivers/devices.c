#include "devices.h"
#include "drivers/ata.h"

void scan_devices()
{
    // KEYBOARD
    // MOUSE

    ata_init();
        // HDD
        // CDROM

    // USB

    // NETWORK
        // E1000
        // PCNET
        // RTL8139
        // RTL8168
}
