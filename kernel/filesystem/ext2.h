#ifndef EXT2_H
#define EXT2_H

#include "util/util.h"

typedef struct
{
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
} superblock_base_t;

typedef struct
{
    superblock_base_t base_fields;
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
} __attribute__((aligned(1024))) superblock_extended_t;

typedef struct
{
    uint32_t block_bitmap_addr;
    uint32_t inode_bitmap_addr;
    uint32_t inode_table_addr;
    uint16_t unalloc_blocks;
    uint16_t unalloc_inodes;
    uint16_t dir_count;
} __attribute__((aligned(32))) blockgroup_descriptor_t;

typedef struct
{
    superblock_extended_t superblock;
    uint32_t block_size;
} ext2fd_t;

void install_ext2(void);

#endif // EXT2_H
