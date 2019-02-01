/*
 * ULMIX ext2 filesystem driver
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Rainer Rudigier
 */

#ifndef EXT2_H
#define EXT2_H

#include <util/util.h>

#define EXT2_NDIR_BLOCKS        12
#define EXT2_IND_BLOCK          EXT2_NDIR_BLOCKS
#define EXT2_DIND_BLOCK         (EXT2_IND_BLOCK + 1)
#define EXT2_TIND_BLOCK         (EXT2_DIND_BLOCK + 1)
#define EXT2_N_BLOCKS           (EXT2_TIND_BLOCK + 1)

#define EXT2_NAME_LEN           255
#define EXT2_IND_BLOCK_LEN      256
#define VFS_INODE_BLOCK_TABLE_LEN 128

struct inode_block_table
{
    // add flags for caching of this information

    struct direntry_struct  *file;                  // which file does the table belong to

    uint32_t block_count_total;                     // blocks total of file
    uint32_t block_count;                           // blocks in current struct
    uint32_t blocks[VFS_INODE_BLOCK_TABLE_LEN];     // inode blocks

    struct inode_block_table *next;                 // next block
};

struct gd_struct // block group descriptor struct
{
    uint32_t bg_block_bitmap;         /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;         /* Inodes bitmap block */
    uint32_t bg_inode_table;          /* Inodes table block */
    uint16_t bg_free_blocks_count;    /* Free blocks count */
    uint16_t bg_free_inodes_count;    /* Free inodes count */
    uint16_t bg_used_dirs_count;      /* Directories count */
} __attribute__((aligned(32)));

struct sb_struct
{
    struct file_struct *fd;             // used to access the block device
    struct filesystem_struct *fs;       // file system file operations

    unsigned long block_size;

    struct gd_struct *group_descriptors;    // array of group descriptors
    unsigned long s_groups_count;           // Number of groups in the fs

    unsigned long s_inodes_total;
    unsigned long s_blocks_total;

    unsigned long s_frag_size;          /* Size of a fragment in bytes */
    unsigned long s_frags_per_block;    /* Number of fragments per block */
    unsigned long s_inodes_per_block;   /* Number of inodes per block */
    unsigned long s_frags_per_group;    /* Number of fragments in a group */
    unsigned long s_blocks_per_group;   /* Number of blocks in a group */
    unsigned long s_inodes_per_group;   /* Number of inodes in a group */
    unsigned long s_itb_per_group;      /* Number of inode table blocks per group */
    unsigned long s_gdb_count;          /* Number of group descriptor blocks */
    unsigned long s_desc_per_block;     /* Number of group descriptors per block */
    unsigned long s_overhead_last;      /* Last calculated overhead */
    unsigned long s_blocks_last;        /* Last seen block count */

    void * s_es; /* Pointer to the super block in the buffer */
};

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
} __attribute__((packed)) superblock_base_t;

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
} __attribute__((aligned(1024))) superblock_extended_t;

typedef struct
{
    uint16_t  i_mode;           /* File mode */
    uint16_t  i_uid;            /* Low 16 bits of Owner Uid */
    uint32_t  i_size;           /* Size in bytes */
    uint32_t  i_atime;          /* Access time */
    uint32_t  i_ctime;          /* Creation time */
    uint32_t  i_mtime;          /* Modification time */
    uint32_t  i_dtime;          /* Deletion Time */
    uint16_t  i_gid;            /* Low 16 bits of Group Id */
    uint16_t  i_links_count;    /* Links count */
    uint32_t  i_blocks;         /* Blocks count */
    uint32_t  i_flags;          /* File flags */

    union {
        struct {
            uint32_t  l_i_reserved1;
        } linux1;
        struct {
            uint32_t  h_i_translator;
        } hurd1;
        struct {
            uint32_t  m_i_reserved1;
        } masix1;
    } osd1;             /* OS dependent 1 */
    uint32_t  i_block[EXT2_N_BLOCKS];/* Pointers to blocks */
    uint32_t  i_generation;   /* File version (for NFS) */
    uint32_t  i_file_acl; /* File ACL */
    uint32_t  i_dir_acl;  /* Directory ACL */
    uint32_t  i_faddr;    /* Fragment address */
    union {
        struct {
            uint8_t    l_i_frag;   /* Fragment number */
            uint8_t    l_i_fsize;  /* Fragment size */
            uint16_t   i_pad1;
            uint16_t  l_i_uid_high;   /* these 2 fields    */
            uint16_t  l_i_gid_high;   /* were reserved2[0] */
            uint32_t   l_i_reserved2;
        } linux2;
        struct {
            uint8_t    h_i_frag;   /* Fragment number */
            uint8_t    h_i_fsize;  /* Fragment size */
            uint16_t  h_i_mode_high;
            uint16_t  h_i_uid_high;
            uint16_t  h_i_gid_high;
            uint32_t  h_i_author;
        } hurd2;
        struct {
            uint8_t    m_i_frag;   /* Fragment number */
            uint8_t    m_i_fsize;  /* Fragment size */
            uint16_t   m_pad1;
            uint32_t   m_i_reserved2[2];
        } masix2;
    } osd2;             /* OS dependent 2 */
} __attribute__((packed)) ext2_inode_t;

typedef struct
{
    uint32_t i_indirect_ptr[EXT2_IND_BLOCK_LEN];
} ext2_inode_blk_table_t;

struct ext2_direntry_struct
{
    uint32_t  inode;          // inode number
    uint16_t  rec_len;        // directory entry length
    uint8_t   name_len;       // name length
    uint8_t   file_type;      // type or MSB of name length
} __attribute__((packed));

void install_ext2(void);

#endif // EXT2_H
