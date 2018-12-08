#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"
#include "drivers/devices.h"
#include "filesystem/filesystem.h"
#include "filesystem/fs_syscalls.h"


static int ext2_probe(struct gendisk_struct *bd, int partition);
static int ext2_mount(const char *mountpoint, struct gendisk_struct *bd, int partition);

void install_ext2()
{
    filesystem_t *ext2fs = kmalloc(sizeof(filesystem_t), 1, "filesystem_t");

    ext2fs->fs_probe = ext2_probe;
    ext2fs->fs_mount = ext2_mount;
    ext2fs->name = "ext2";

    install_fs(ext2fs);
}

static void get_superblock(struct gendisk_struct *bd, struct hd_struct *partition, unsigned char *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    bd->fops.seek(bd->drv_struct, partition->sector_offset + (superblock_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)buf, superblock_size / 512);
}

static int ext2_probe(struct gendisk_struct *bd, int partition)
{
    unsigned char superblock[1024];
    get_superblock(bd, &(bd->part_list[partition]), superblock);
    if (superblock[0x38] == 0x53 && superblock[0x39] == 0xef)
        return 0;
    return -1;
}

static int ext2_mount(const char *mountpoint, struct gendisk_struct *bd, int partition)
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
    get_superblock(bd, &(bd->part_list[partition]), (unsigned char*)&(ext2descriptor->superblock));
    ext2descriptor->block_size = 1024 << ext2descriptor->superblock.base_fields.block_size_log;


    // for now
    return 0;
}
