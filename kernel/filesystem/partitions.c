#include "partitions.h"
#include "filesystem/path.h"
#include "memory/kheap.h"
#include "filesystem/filesystem.h"
#include "log.h"

typedef struct
{
    dev_t *device;
    size_t part_offset;
} part_t;

static fd_t *part_open(file_t *file);
static int part_close(fd_t *fd);
static ssize_t part_read(fd_t *fd, char *buf, size_t count);
static ssize_t part_write(fd_t *fd, char *buf, size_t count);
static ssize_t part_seek(fd_t *fd, size_t offset, int whence);
static void prepare_mount(fd_t *device, mbr_entry_t mbr);
static int is_mbr(unsigned char *sec1);



int install_disk(fd_t *device)
{
    // determine partition table format:
    if (device == NULL)
    {
        klog(KLOG_DEBUG, "install_disk(): could not read device");
        return -1;
    }

    // 1. load Master Boot Record (first sector)
    unsigned char mbr[512];
    device->read(device, (char*)mbr, 1);
    if (!is_mbr(mbr))
    {
        klog(KLOG_DEBUG, "install_disk(): partition table format not recognized");
        return -1;
    }

    // read MBR partition table
    klog(KLOG_DEBUG, "install_disk(): reading MBR partition table:");
    for (int i = 0; i < 4; i++)
    {
        mbr_entry_t entry;
        int entry_offset = 0x1be + 16 * i;
        entry.bootable = (mbr[entry_offset] & 0x80) ? 1 : 0;
        entry.is_lba48 = (mbr[entry_offset] & 0x01) ? 1 : 0;

        entry.id = i;
        entry.system_id = mbr[entry_offset + 4];
        entry.start_sector = *((uint32_t*)(mbr + entry_offset + 8));
        entry.sector_count = *((uint32_t*)(mbr + entry_offset + 12));

        if (entry.sector_count > 0 && entry.start_sector > 0)
        {
            prepare_mount(device, entry);

            // for now, only mount one partition - we need some sort of fstab first
            // how else do we know where to mount the partition
            break;
        }
    }

    return 0;
}

static void prepare_mount(fd_t *device, mbr_entry_t mbr)
{
    // create device file for partition
    char part_name[64];
    strcpy(part_name, device->file->name);
    strcat(part_name, "-");
    itoa(mbr.id, (char*)(part_name + strlen(part_name)));

    part_t *part = kmalloc(sizeof(part_t), 1, "part_t");
    part->device = device->file->drv_struct;
    part->part_offset = mbr.start_sector;

    file_t f =
    {
        .type = FILE_DEVICE,
        .io_size = device->file->io_size,
        .drv_struct = part,

        .open = part_open,
        .close = part_close,
        .size = mbr.sector_count,

        .group = 0,
        .owner = 0,
        .permissions = PERM_USER_READ | PERM_USER_WRITE
    };
    strcpy(f.name, part_name);

    fnode_t *fpart;
    if ((fpart = mknod(get_node("/dev"), f)) == NULL)
    {
        klog(KLOG_WARN, "error: failed to register device file /dev/%s", f.name);
    }

    // Default mount point -> fstab!
    fnode_t *mount_point = get_node("/");
    fd_t *partfd = fpart->meta.open(&fpart->meta);
    if (part_mount(mount_point, partfd, mbr) < 0)
    {
        klog(KLOG_WARN, "mbr partition #%d could not be mounted", mbr.id);
    }
}

static int is_mbr(unsigned char *sec1)
{
    if (sec1[0x1fe] == 0x55 && sec1[0x1ff] == 0xAA)
    {
        return 1;
    }

    return 0;
}

static fd_t *part_open(file_t *file)
{
    fd_t *fdev = kmalloc(sizeof(fd_t), 1, "fd_t");
    fdev->file = file;
    fdev->seek_offset = 0;
    fdev->read = part_read;
    fdev->write = part_write;
    fdev->seek = part_seek;
    fdev->fdi_struct = NULL;
    return fdev;
}

static int part_close(fd_t *fd)
{
    kfree(fd);
    return 0;
}

static ssize_t part_read(fd_t *fd, char *buf, size_t count)
{
    part_t *part = (part_t*)(fd->file->drv_struct);
    part->device->seek(part->device->drv_struct, part->part_offset + fd->seek_offset, SEEK_SET);
    return part->device->read(part->device->drv_struct, buf, count);
}

static ssize_t part_write(fd_t *fd, char *buf, size_t count)
{
    part_t *part = (part_t*)(fd->file->drv_struct);
    part->device->seek(part->device->drv_struct, part->part_offset + fd->seek_offset, SEEK_SET);
    return part->device->write(part->device->drv_struct, buf, count);
}

static ssize_t part_seek(fd_t *fd, size_t offset, int whence)
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
