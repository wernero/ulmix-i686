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



void register_device(device_type_t type, void *drv_struct, size_t io_size,
            ssize_t (*read) (void* drv_struct, char *buf, size_t count),
            ssize_t (*write)(void* drv_struct, char *buf, size_t count),
            ssize_t (*seek) (void* drv_struct, size_t offset, int whence),
            char *dev_name)
{
    file_t f =
    {
        .type = FILE_DEVICE,
        .io_size = io_size,
        .drv_struct = drv_struct,
        .read = read,
        .write = write,
        .seek = seek,

        .group = 0,
        .owner = 0,
        .permissions = PERM_USER_READ | PERM_USER_WRITE
    };
    strcpy(f.name, dev_name);

    fnode_t *dev;
    if ((dev = mknod(get_node("/dev"), f)) == NULL)
    {
        klog(KLOG_WARN, "error: failed to register device file /dev/%s", f.name);
    }

    mount(get_node("/"), &f);
}
