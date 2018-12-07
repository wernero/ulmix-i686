#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"


static int ext2_probe(fd_t *device);
static int ext2_mount(fnode_t *mountpoint, fd_t *device);
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

static void get_superblock(fd_t *device, unsigned char *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    device->seek(device, superblock_offset / device->file->io_size, SEEK_SET);
    device->read(device, (char*)buf, superblock_size / device->file->io_size);
}

static int ext2_probe(fd_t *device)
{
    unsigned char superblock[1024];
    get_superblock(device, superblock);
    if (superblock[0x38] == 0x53 && superblock[0x39] == 0xef)
        return 0;
    return -1;
}

static int ext2_mount(fnode_t *mountpoint, fd_t *device)
{
    /*
     *                          --- HDD DRIVER ---
     *
     * I/O Sector size:         device->file->io_size
     * Current seek offset:     device->seek_offset
     * read from HDD:           device->read(device, <buffer>, <sector-count>)
     * change seek offset:      device->seek(device, <sector-offset>, SEEK_SET)
     *                          device->seek(device, <sector-offset>, SEEK_CUR)
     *
     * write to HDD:            device->write(device, <buffer>, <sector-count>)
     *                          (not tested yet, but should work)
     *
     *                           --- FILESYSTEM ---
     *
     * the filesystem expects the ext2 driver to create a model of fnode_t's of the
     * ext2 tree, when mount() is called.
     *
     * create an fnode_t:       mknod(fnode_t parent, file_t new_node)
     */

    // get superblock and store in the ext2 descriptor
    ext2fd_t *ext2descriptor = kmalloc(sizeof(ext2fd_t), 1, "ext2fd_t");
    get_superblock(device, (unsigned char*)&(ext2descriptor->superblock));
    ext2descriptor->block_size = 1024 << ext2descriptor->superblock.base_fields.block_size_log;


    // for now
    return 0;
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
