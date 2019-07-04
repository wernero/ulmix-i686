/*
 * ULMIX ext2 filesystem driver
 * Copyright (C) 2018-2019
 *
 *  Rainer Rudigier <rainer@rudigier.at>
 *  Alexander Ulmer <alexulmer1999@gmail.com>
 *
 * this is the original implementation of the ext2 filesystem
 * from December 2018, where the majority of the code has been
 * written by Rainer Rudigier.
 */

#include <fs.h>
#include <vfs.h>
#include <errno.h>
#include <debug.h>
#include <mem.h>

#define BLOCK_SIZE  512
#define SB_SECT_OFFSET  2
#define SB_SECT_SIZE    2

typedef struct
{
    // base fields
    uint32_t total_inodes;
    uint32_t total_blocks;
    uint32_t root_blocks;
    uint32_t unalloc_blocks;
    uint32_t unalloc_inodes;
    uint32_t superblock;
    uint32_t block_size_log;
    uint32_t fragment_size_log;
    uint32_t blocks_per_group;
    uint32_t fragments_per_group;
    uint32_t inodes_per_group;
    uint32_t last_mount;
    uint32_t last_write;
    uint16_t mounts_since_check;
    uint16_t mounts_remaining;
    uint16_t signature;
    uint16_t fs_state;
    uint16_t error_action;
    uint16_t minor_version;
    uint32_t last_check;
    uint32_t check_interval;
    uint32_t creator_os;
    uint32_t major_version;
    uint16_t reserved_user;
    uint16_t reserved_group;

    // extended fields
    uint32_t first_inode_non_reserved;
    uint16_t inode_size;
    uint16_t backup_blockgroup;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t readonly_features;
    char fs_id[16];
    char vol_name[16];
    char last_mount_path[64];
    uint32_t compression_alg;
    uint8_t prealloc_file_blocks;
    uint8_t prealloc_dir_blocks;
    uint16_t unused;
    char journal_id[16];
    uint32_t journal_inode;
    uint32_t journal_device;
    uint32_t orphan_head;
} __attribute__((aligned(1024))) sb_t;

struct gd_struct
{
    uint32_t bg_block_bitmap;       /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;       /* Inodes bitmap block */
    uint32_t bg_inode_table;        /* Inodes table block */
    uint16_t bg_free_blocks_count;  /* Free blocks count */
    uint16_t bg_free_inodes_count;  /* Free inodes count */
    uint16_t bg_used_dirs_count;    /* Directories count */
} __attribute__((aligned(32)));

struct ext2fs_struct
{
    sb_t superblock;
    struct hd_struct *part;

    unsigned long block_size;

    // group descriptor table:
    unsigned long gd_count;
    struct gd_struct *group_descriptors;
};

static ssize_t hdd_read(struct ext2fs_struct *fs, unsigned char *buffer,
                        size_t sectors, size_t lba)
{
    return fs->part->bd->fops.read(fs->part->bd->drv_struct,
                     buffer, sectors, lba + fs->part->sect_offset);
}

// commented out as long as not used
/*static ssize_t hdd_write(struct ext2fs_struct *fs, unsigned char *buffer,
                        size_t sectors, size_t lba)
{
    return fs->part->bd->fops.write(fs->part->bd->drv_struct,
                     buffer, sectors, lba + fs->part->sect_offset);
}*/

static int ext2_probe(struct hd_struct *part)
{
    sb_t sb;
    struct ext2fs_struct fs = { .part = part };

    // fetch ext2 superblock from disk
    if (hdd_read(&fs, (unsigned char *)&sb,
            SB_SECT_SIZE, SB_SECT_OFFSET) < SB_SECT_SIZE)
        return -EIO;

    // if the signature matches, we're in
    uint16_t signature = sb.signature;
    if (signature == 0xef53)
        return SUCCESS;

    // otherwise, there's no ext2fs
    return -ENOTSUP;
}

static int ext2_get_direntries(struct dir_struct *dir)
{
    return SUCCESS;
}

static const struct fs_struct ext2fs;
static int ext2_mount(struct hd_struct *part, struct dir_struct *mnt_point)
{
    int error;
    if ((error = ext2_probe(part)) < 0)
        return error;

    // create the data structures describing the file system
    struct ext2fs_struct *fsinfo = kmalloc(sizeof(struct ext2fs_struct), 1, "ext2_info");
    fsinfo->part = part;

    struct mntp_struct *mnt_info = kmalloc(sizeof(struct mntp_struct), 1, "mntp_struct");
    mnt_info->part = part;
    mnt_info->fs_info = &ext2fs;
    mnt_info->fs_data = fsinfo;

    // fetch the superblock from disk
    if ((error = hdd_read(fsinfo, (unsigned char*)&fsinfo->superblock,
                 SB_SECT_SIZE, SB_SECT_OFFSET)) < SB_SECT_SIZE)
        goto no_mount;

    // initialize some variables
    fsinfo->block_size = 0x400 << fsinfo->superblock.block_size_log;
    fsinfo->gd_count = fsinfo->superblock.total_blocks
            / fsinfo->superblock.blocks_per_group + 1;

    // locate the group descriptor table and fetch it into memory
    unsigned gdt_lba = (fsinfo->block_size == 0x400) ? 2 : 1;
    unsigned gdt_size = ((fsinfo->gd_count * sizeof(struct gd_struct)) / BLOCK_SIZE) + 1;

    fsinfo->group_descriptors = kmalloc(gdt_size * BLOCK_SIZE,
                                           1, "ext2 gd_struct[]");
    if ((error = hdd_read(fsinfo, (unsigned char*)fsinfo->group_descriptors,
                          gdt_size, gdt_lba)) < 0)
        goto no_mount;

    // fetch the root inode and it's immediate children
    mnt_point->inode_no = 2;
    if ((error = ext2_get_direntries(mnt_point)) < 0)
        goto no_mount;

    // define the directory as mount point and give it the
    // information on where to find filesystem functions
    mnt_point->fs_info = mnt_info;
    mnt_point->mnt_point = mnt_point;

    return SUCCESS;

no_mount:
    kfree(fsinfo);
    kfree(mnt_info);
    return error;
}

static const struct fs_struct ext2fs = {
    .name = "ext2",
    .mbr_id = 0x83,
    .fops = {
        .fs_probe = ext2_probe,
        .fs_mount = ext2_mount,
        .fs_get_direntries = ext2_get_direntries
    }
};

void __init init_ext2()
{
    register_fs(&ext2fs);
}

