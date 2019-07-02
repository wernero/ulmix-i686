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

struct ext2fs_struct
{
    struct hd_struct *part;

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
            SB_SECT_OFFSET, SB_SECT_SIZE) < SB_SECT_SIZE)
        return -EIO;

    // if the signature matches, we're in
    uint16_t signature = sb.signature;
    if (signature == 0xef53)
        return SUCCESS;

    // otherwise, there's no ext2fs
    return -ENOTSUP;
}

static const struct fs_struct ext2_info = {
    .name = "ext2",
    .mbr_id = 0x83,
    .fops = {
        .fs_probe = ext2_probe
    }
};

void __init init_ext2()
{
    register_fs(&ext2_info);
}

