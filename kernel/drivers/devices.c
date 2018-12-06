#include "devices.h"
#include "drivers/ata.h"
#include "memory/kheap.h"
#include "log.h"

static fd_t *dev_open(file_t *file);
static int dev_close(fd_t *fd);
static ssize_t dev_read(fd_t *fd, char *buf, size_t count);
static ssize_t dev_write(fd_t *fd, char *buf, size_t count);
static ssize_t dev_seek(fd_t *fd, size_t offset, int whence);

void scan_devices()
{
    ata_init();
    // ... more devices
}



static fd_t *dev_open(file_t *file)
{
    fd_t *fdev = kmalloc(sizeof(fd_t), 1, "fd_t");
    fdev->file = file;
    fdev->seek_offset = 0;
    fdev->read = dev_read;
    fdev->write = dev_write;
    fdev->seek = dev_seek;
    fdev->fdi_struct = NULL;
    return fdev;
}

static int dev_close(fd_t *fd)
{
    kfree(fd);
    return 0;
}

void register_device(device_type_t type, void *drv_struct, size_t io_size,
            ssize_t (*read) (void* drv_struct, char *buf, size_t count),
            ssize_t (*write)(void* drv_struct, char *buf, size_t count),
            ssize_t (*seek) (void* drv_struct, size_t offset, int whence),
            char *dev_name)
{
    dev_t *dev = kmalloc(sizeof(dev_t), 1, "dev_t");
    dev->drv_struct = drv_struct;
    dev->read = read;
    dev->write = write;
    dev->seek = seek;

    file_t f =
    {
        .type = FILE_DEVICE,
        .io_size = io_size,
        .drv_struct = dev,

        .open = dev_open,
        .close = dev_close,

        .group = 0,
        .owner = 0,
        .permissions = PERM_USER_READ | PERM_USER_WRITE
    };
    strcpy(f.name, dev_name);

    fnode_t *devn;
    if ((devn = mknod(get_node("/dev"), f)) == NULL)
    {
        klog(KLOG_WARN, "error: failed to register device file /dev/%s", f.name);
    }

    fd_t *device = f.open(&f);
    if (disk_mount(get_node("/"), device) < 0)
    {
        klog(KLOG_WARN, "error: could not mount device /dev/%s", f.name);
    }
}

static ssize_t dev_read(fd_t *fd, char *buf, size_t count)
{
    dev_t *dev = (dev_t*)(fd->file->drv_struct);
    dev->seek(dev->drv_struct, fd->seek_offset, SEEK_CUR);
    return dev->read(dev->drv_struct, buf, count);
}

static ssize_t dev_write(fd_t *fd, char *buf, size_t count)
{
    dev_t *dev = (dev_t*)(fd->file->drv_struct);
    dev->seek(dev->drv_struct, fd->seek_offset, SEEK_CUR);
    return dev->write(dev->drv_struct, buf, count);
}

static ssize_t dev_seek(fd_t *fd, size_t offset, int whence)
{
    switch (whence)
    {
    case SEEK_SET:
        fd->seek_offset = offset;
        break;
    case SEEK_CUR:
        fd->seek_offset += offset;
        break;
    case SEEK_END:
    default:
        return -1;
    }
    return 0;
}
