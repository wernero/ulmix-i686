#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"
#include "drivers/devices.h"
#include "filesystem/filesystem.h"
#include "filesystem/fs_syscalls.h"
#include "filesystem/vfscore.h"


static int ext2_probe(struct gendisk_struct *bd, int partition);
static int ext2_mount(struct dir_struct *mountpoint, struct gendisk_struct *bd, int part);
static int ext2_get_direntry(struct dir_struct *miss);
static int ext2_get_inode(struct dir_struct *parent, unsigned long inode_no);


void install_ext2()
{
    filesystem_t *ext2fs = kmalloc(sizeof(filesystem_t), 1, "filesystem_t");

    ext2fs->fs_probe = ext2_probe;
    ext2fs->fs_mount = ext2_mount;
    ext2fs->fs_get_direntry = ext2_get_direntry;
    ext2fs->fs_get_inode = ext2_get_inode;
    ext2fs->name = "ext2";

    install_fs(ext2fs);
}

static void get_superblock(struct gendisk_struct *bd, struct hd_struct *partition, superblock_extended_t *buf)
{
    int superblock_offset = 0x400;
    int superblock_size = 0x400;

    bd->fops.seek(bd->drv_struct, partition->sector_offset + (superblock_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)buf, superblock_size / 512);
}

static int ext2_probe(struct gendisk_struct *bd, int partition)
{
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");

    get_superblock(bd, &(bd->part_list[partition]), superblock);

    klog(KLOG_INFO, "ext2_probe(): signature=%x",
        superblock->signature
        );

    if (superblock->signature == 0xef53)
        return 0;
    return -1;
}

static int ext2_mount(struct dir_struct *mountpoint, struct gendisk_struct *bd, int part)
{
    // get inode 2
    // parse & fill out
    // superblock layout to be read from disk
    superblock_extended_t *superblock = kmalloc(sizeof(superblock_extended_t), 1, "superblock_extended_t");

    struct hd_struct *partition = &(bd->part_list[part]);
    char *group_descriptor_buf;
    blockgroup_descriptor_t group_descriptor;
    int group_descriptor_offset = 0x800;
    int group_descriptor_size = 0x0;

    int i;


    // get superblock from partition
    get_superblock(bd, partition, superblock);



    klog(KLOG_INFO, "ext2_mount(): signature=%x, total_inodes=%d, total_blocks=%d, unalloc_inodes=%d, unalloc_blocks=%d",
        superblock->signature,
        superblock->total_inodes,
        superblock->total_blocks,
        superblock->unalloc_inodes,
        superblock->unalloc_blocks
        );



    // initalize sb_struct in dir_struct
    mountpoint->sb = kmalloc(sizeof(struct sb_struct), 1 , "sb_struct");

    mountpoint->sb->s_blocks_total = superblock->total_blocks;
    mountpoint->sb->s_inodes_total = superblock->total_inodes;

    mountpoint->sb->s_blocks_per_group = superblock->blocks_per_group;
    mountpoint->sb->s_inodes_per_group = superblock->inodes_per_group;

    // how many blocks do we have
    mountpoint->sb->s_gdb_count = superblock->total_blocks / superblock->blocks_per_group + 1; // check roundup 

//    group_descriptor_size = (((mountpoint->sb->s_gdb_count * sizeof(blockgroup_descriptor_t)) / 0x200) + 1) * 0x200;
    group_descriptor_size = 0x400;
    
    group_descriptor_buf = kmalloc(group_descriptor_size, 1, "group_descriptor_buf");

    bd->fops.seek(bd->drv_struct, partition->sector_offset + (group_descriptor_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)group_descriptor_buf, group_descriptor_size / 512);


    klog(KLOG_INFO, "ext2_mount(): gdb=%x, gdc=%x, gds=%x, gdo=%x",
        group_descriptor_buf,
        mountpoint->sb->s_gdb_count,
        group_descriptor_size,
        partition->sector_offset + (group_descriptor_offset / 512)
        );

    for(i=0; i < mountpoint->sb->s_gdb_count; i++) {

        memcpy(&group_descriptor,(group_descriptor_buf + i * 0x20), 0x20);

        klog(KLOG_INFO, "ext2_mount(): bba=%x, iba=%x, ita=%x, ub=%d, ui=%d, dc=%d",
            group_descriptor.block_bitmap_addr,
            group_descriptor.inode_bitmap_addr,
            group_descriptor.inode_table_addr,
            group_descriptor.unalloc_blocks,
            group_descriptor.unalloc_inodes,
            group_descriptor.dir_count
            );


    }



    // for now
    return 0;
}

static int ext2_get_direntry(struct dir_struct *miss)
{
    // TODO
    return 0;
}

static int ext2_get_inode(struct dir_struct *parent, unsigned long inode_no)
{
    // TODO
    return 0;
}
