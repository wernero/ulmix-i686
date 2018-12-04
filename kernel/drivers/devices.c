#include "devices.h"
#include "drivers/ata.h"
#include "log.h"

void scan_devices()
{
    klog(KLOG_DEBUG, "scanning devices");
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
