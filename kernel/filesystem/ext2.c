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

static void get_superblock(fd_t *device, mbr_entry_t part, unsigned char *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    device->seek(device, part.start_sector + (superblock_offset / device->file->io_size), SEEK_SET);
    device->read(device, (char*)buf, superblock_size / device->file->io_size);
}

/*static blockgroup_descriptor_t get_group_descriptor(int index, fd_t *device, ext2fd_t *part)
{
    blockgroup_descriptor_t group_descriptor;
    unsigned long blockg_desc_table_offset = part->block_size;
    if (part->block_size == 1024)
    {
        blockg_desc_table_offset = 2048;
    }

    // load group descriptor table from disk, and return
    // the entry with the corresponding index

    return group_descriptor;
}*/

static int ext2_probe(fd_t *device, mbr_entry_t part)
{
    unsigned char superblock[1024];
    get_superblock(device, part, superblock);
    if (superblock[0x38] == 0x53 && superblock[0x39] == 0xef)
        return 0;
    return -1;
}

static int ext2_mount(fnode_t *mountpoint, fd_t *device, mbr_entry_t part)
{
    /*
     *                          --- HDD DRIVER ---
     * please note that 'fd_t device' is the file descriptor for the raw hard disk
     * -> does not respect partition boundaries! Always add mbr_entry_t.start_sector
     * as offset when performing an i/o operation on the hard disk!
     *
     * I/O Sector size:         device->file->io_size
     * Current seek offset:     device->seek_offset
     * read from HDD:           device->read(device, <buffer>, <sector-count>)
     * change seek offset:      device->seek(device, <sector-offset>, SEEK_SET)
     *                          device->seek(device, <sector-offset>, SEEK_CUR)
     *
     * write to HDD:            device->write(device, <buffer>, <sector-count>)
     *                          (not tested yet, but should work)
     */

    // get superblock and store in the ext2 descriptor
    ext2fd_t *ext2descriptor = kmalloc(sizeof(ext2fd_t), 1, "ext2fd_t");
    get_superblock(device, part, (unsigned char*)&(ext2descriptor->superblock));
    ext2descriptor->block_size = 1024 << ext2descriptor->superblock.base_fields.block_size_log;
    ext2descriptor->part_offset = part.start_sector;


    /*
    // list root directory
    int inode = 2;
    int root_block_grp = (inode - 1) / ext2descriptor->superblock.base_fields.inodes_per_group;
    blockgroup_descriptor_t bgroup = get_group_descriptor(root_block_grp, device, ext2descriptor);

    unsigned long inode_size = ext2descriptor->superblock.inode_size;
    if (ext2descriptor->superblock.base_fields.major_version < 1)
        inode_size = 128;

    int inode_table_offset = (inode - 1) % ext2descriptor->superblock.base_fields.inodes_per_group;

    uint32_t inode_sector = (ext2descriptor->block_size / device->file->io_size) * bgroup.inode_table_addr;
    */



    cli();
    hlt();
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
