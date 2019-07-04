#include <init.h>

extern void init_ata();
extern void init_ext2();
extern void init_vga();

extern void setup_fs();
extern void setup_pci();

void __init iosetup()
{
    // Setup all the drivers
    init_ata();
    init_ext2();
    init_vga();

    setup_fs();

    // perform PCI enumeration
    // PCI drivers should already be in place
    setup_pci();
}
