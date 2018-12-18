#include "ext2.h"
#include "memory/kheap.h"
#include "log.h"
#include "drivers/devices.h"
#include "filesystem/filesystem.h"
#include "filesystem/fs_syscalls.h"
#include "filesystem/vfscore.h"

#define EXT2_BLOCK_SIZE         0x400

static int ext2_probe(struct gendisk_struct *bd, int partition);
static int ext2_mount(struct dir_struct *mountpoint, struct gendisk_struct *bd, int part);
static int ext2_get_direntry(struct dir_struct *miss);
static int ext2_get_inode(struct dir_struct *parent, unsigned long inode_no);


void install_ext2()
{
    struct filesystem_struct *ext2fs = kmalloc(sizeof(struct filesystem_struct), 1, "filesystem_struct");

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

  //  struct hd_struct *partition = &(bd->part_list[part]);
    char *group_descriptor_buf;
    blockgroup_descriptor_t group_descriptor;
    int group_descriptor_offset = 0x800;
    int group_descriptor_size = 0x0;
    struct gd_struct * current_gds;
    {

    };

    int i;


    mountpoint->bd = bd;
    mountpoint->partition = &(bd->part_list[part]);

    // get superblock from partition
    get_superblock(bd, mountpoint->partition, superblock);



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
    mountpoint->sb->s_gdb_count = superblock->total_blocks / superblock->blocks_per_group + 1; // **TODO** correct rouund up

    group_descriptor_size = (((mountpoint->sb->s_gdb_count * sizeof(blockgroup_descriptor_t)) / 0x200) + 1) * 0x200;
//    group_descriptor_size = 0x400;

    group_descriptor_buf = kmalloc(group_descriptor_size, 1, "group_descriptor_buf");

    bd->fops.seek(bd->drv_struct, mountpoint->partition->sector_offset + (group_descriptor_offset / 512), SEEK_SET);
    bd->fops.read(bd->drv_struct, (char*)group_descriptor_buf, group_descriptor_size / 512);


    klog(KLOG_INFO, "ext2_mount(): gdb=%x, gdc=%x, gds=%x, gdo=%x",
        group_descriptor_buf,
        mountpoint->sb->s_gdb_count,
        group_descriptor_size,
        mountpoint->partition->sector_offset + (group_descriptor_offset / 512)
        );

    mountpoint->sb->s_group_desc = kmalloc(sizeof(struct gd_struct),1,"gd_struct");
    current_gds = mountpoint->sb->s_group_desc;


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

        current_gds->bg_block_bitmap = group_descriptor.block_bitmap_addr;          /* Blocks bitmap block */
        current_gds->bg_inode_bitmap = group_descriptor.inode_bitmap_addr;          /* Inodes bitmap block */
        current_gds->bg_inode_table = group_descriptor.inode_table_addr;            /* Inodes table block */
        current_gds->bg_free_blocks_count = group_descriptor.unalloc_blocks;        /* Free blocks count */
        current_gds->bg_free_inodes_count = group_descriptor.unalloc_inodes;        /* Free inodes count */
        current_gds->bg_used_dirs_count = group_descriptor.dir_count;               /* Directories count */
        current_gds->bg_next = 0x0;

        if(i < (mountpoint->sb->s_gdb_count-1)) { // no kmalloc if last element

            current_gds->bg_next = kmalloc(sizeof(struct gd_struct),1,"gd_struct");
            current_gds = current_gds->bg_next;
        }

    }

    mountpoint->inode_no = 2; // root inode of a partition is always 2


    ext2_get_direntry(mountpoint); // get directories entries for


    // for now
    return 0;
}

static int ext2_get_direntry(struct dir_struct *miss)
{
    // TODO

    int block_group = 0;
    struct gd_struct * gds;
    int i;

    int inode_group_offset = 0;

    char *inode_buf = kmalloc(0x200, 1, "inode_buf");
    char *direntry_buf = kmalloc(0x400, 1, "direntry_buf");
    ext2_inode_t *inode = kmalloc(sizeof(ext2_inode_t), 1, "ext2_inode_t");

    ext2_direntry2_t *de = kmalloc(sizeof(ext2_direntry2_t), 1, "ext2_direntry2_t");

    // which inode do we want to ready
    // which block group is required - inode / inode per block
    block_group =  miss->inode_no / miss->sb->s_inodes_per_group;

    gds = miss->sb->s_group_desc; // this is group descriptor 0;

    for(i=1; i < block_group; i++){
        gds = gds->bg_next;
    }

    klog(KLOG_INFO, "ext2_get_direntry(): gds0=%x, gds=%x, blkgrp=%x, ita=%x, ita_off=%x",
        miss->sb->s_group_desc,
        gds,
        block_group,
        gds->bg_inode_table,
        gds->bg_inode_table * EXT2_BLOCK_SIZE
        );

    // there are 4 inodes structs (0x80) (inode_groups) per disk block (0x200) - calculate the offset of the inode address table beginning

    inode_group_offset = ((miss->inode_no % miss->sb->s_inodes_per_group) / 4) * EXT2_BLOCK_SIZE / 512;

    // get inode address table from respective block and jump to it
    miss->bd->fops.seek(miss->bd->drv_struct, miss->partition->sector_offset + (gds->bg_inode_table * EXT2_BLOCK_SIZE / 512) + inode_group_offset, SEEK_SET);
    miss->bd->fops.read(miss->bd->drv_struct, (char*)inode_buf, 0x200 / 512);

    // get respective inode data

    memcpy(inode,(inode_buf + ((miss->inode_no -1) * 0x80)), 0x80);

    // check if directory 0x4000
    // klog(KLOG_INFO, "ext2_get_direntry(): mode=%x, size=%x, data0=%x, data1=%x",
    //     inode->i_mode,
    //     inode->i_size,
    //     inode->i_block[0] * EXT2_BLOCK_SIZE,
    //     inode->i_block[1] * EXT2_BLOCK_SIZE
    //     );

    // get directory data0
    miss->bd->fops.seek(miss->bd->drv_struct, miss->partition->sector_offset + (inode->i_block[0] * EXT2_BLOCK_SIZE / 512), SEEK_SET);
    miss->bd->fops.read(miss->bd->drv_struct, (char*)direntry_buf, 0x400 / 512);

    for(i=0; i < 0x400;){
        memcpy(&de->inode, direntry_buf+i, sizeof(uint32_t));
        i += sizeof(uint32_t);
        memcpy(&de->rec_len, direntry_buf+i, sizeof(uint16_t));
        i += sizeof(uint16_t);
        memcpy(&de->name_len, direntry_buf+i, sizeof(uint8_t));
        i += sizeof(uint8_t);
        memcpy(&de->file_type, direntry_buf+i, sizeof(uint8_t));
        i += sizeof(uint8_t);
        memcpy(de->name, direntry_buf+i, de->name_len);
        i += (de->rec_len - 8);

        klog(KLOG_INFO, "ext2_get_direntry(): i=%d, inode=%x, rec_len=%x, name_len=%x, file_type=%x, name=%s",
            i,
            de->inode,
            de->rec_len,
            de->name_len,
            de->file_type,
            de->name
            );
    }


    return 0;
}

static int ext2_get_inode(struct dir_struct *parent, unsigned long inode_no)
{
    // TODO
    return 0;
}
