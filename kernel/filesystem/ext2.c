#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"

static int ext2_probe(fd_t *device, mbr_entry_t part);
static int ext2_mount(fnode_t *mountpoint, fd_t *device, mbr_entry_t part);
static fd_t *ext2_open(fnode_t *file);
static ssize_t ext2_read(fd_t *file, char *buf, size_t count);
static ssize_t ext2_write(fd_t *file, char *buf, size_t count);
static int ext2_close(fd_t *file);

void install_ext2()
{
    filesystem_t *ext2fs = kmalloc(sizeof(filesystem_t), 1, "filesystem_t");

    ext2fs->fs_probe =  ext2_probe;
    ext2fs->fs_mount =  ext2_mount;
    ext2fs->fs_open =   ext2_open;
    ext2fs->fs_read =   ext2_read;
    ext2fs->fs_write =  ext2_write;
    ext2fs->fs_close =  ext2_close;
    ext2fs->name = "ext2";

    install_fs(ext2fs);
}

static int ext2_probe(fd_t *device, mbr_entry_t part)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;
    unsigned char superblock[superblock_size];

    device->seek(device, part.start_sector + (superblock_offset / device->file->io_size), SEEK_SET);
    device->read(device, (char*)superblock, superblock_size / device->file->io_size);

    if (superblock[0x38] == 0x53 && superblock[0x39] == 0xef)
        return 0;
    return -1;
}

static int ext2_mount(fnode_t *mountpoint, fd_t *device, mbr_entry_t part)
{
    return -1;
}

static fd_t *ext2_open(fnode_t *file)
{
    return NULL;
}

static ssize_t ext2_read(fd_t *file, char *buf, size_t count)
{
    return -1;
}

static ssize_t ext2_write(fd_t *file, char *buf, size_t count)
{
    return -1;
}

static int ext2_close(fd_t *file)
{
    return -1;
}
