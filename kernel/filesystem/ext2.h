#ifndef EXT2_H
#define EXT2_H

#include "util/util.h"
//#include <linux/types.h>

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




// // from uapi/linux/types.h
// typedef __u16 __bitwise __le16;
// typedef __u16 __bitwise __be16;
// typedef __u32 __bitwise __le32;
// typedef __u32 __bitwise __be32;
// typedef __u64 __bitwise __le64;
// typedef __u64 __bitwise __be64;

// typedef __u16 __bitwise __sum16;
// typedef __u32 __bitwise __wsum;


// /*
//  * Structure of the super block
//  */
// struct ext2_super_block {
//     __le32  s_inodes_count;     /* Inodes count */
//     __le32  s_blocks_count;     /* Blocks count */
//     __le32  s_r_blocks_count;   /* Reserved blocks count */
//     __le32  s_free_blocks_count;    /* Free blocks count */
//     __le32  s_free_inodes_count;    /* Free inodes count */
//     __le32  s_first_data_block; /* First Data Block */
//     __le32  s_log_block_size;   /* Block size */
//     __le32  s_log_frag_size;    /* Fragment size */
//     __le32  s_blocks_per_group; /* # Blocks per group */
//     __le32  s_frags_per_group;  /* # Fragments per group */
//     __le32  s_inodes_per_group; /* # Inodes per group */
//     __le32  s_mtime;        /* Mount time */
//     __le32  s_wtime;        /* Write time */
//     __le16  s_mnt_count;        /* Mount count */
//     __le16  s_max_mnt_count;    /* Maximal mount count */
//     __le16  s_magic;        /* Magic signature */
//     __le16  s_state;        /* File system state */
//     __le16  s_errors;       /* Behaviour when detecting errors */
//     __le16  s_minor_rev_level;  /* minor revision level */
//     __le32  s_lastcheck;        /* time of last check */
//     __le32  s_checkinterval;    /* max. time between checks */
//     __le32  s_creator_os;       /* OS */
//     __le32  s_rev_level;        /* Revision level */
//     __le16  s_def_resuid;       /* Default uid for reserved blocks */
//     __le16  s_def_resgid;       /* Default gid for reserved blocks */
    
//      * These fields are for EXT2_DYNAMIC_REV superblocks only.
//      *
//      * Note: the difference between the compatible feature set and
//      * the incompatible feature set is that if there is a bit set
//      * in the incompatible feature set that the kernel doesn't
//      * know about, it should refuse to mount the filesystem.
//      * 
//      * e2fsck's requirements are more strict; if it doesn't know
//      * about a feature in either the compatible or incompatible
//      * feature set, it must abort and not try to meddle with
//      * things it doesn't understand...
     
//     __le32  s_first_ino;        /* First non-reserved inode */
//     __le16   s_inode_size;      /* size of inode structure */
//     __le16  s_block_group_nr;   /* block group # of this superblock */
//     __le32  s_feature_compat;   /* compatible feature set */
//     __le32  s_feature_incompat;     /* incompatible feature set */
//     __le32  s_feature_ro_compat;    /* readonly-compatible feature set */
//     __u8    s_uuid[16];     /* 128-bit uuid for volume */
//     char    s_volume_name[16];  /* volume name */
//     char    s_last_mounted[64];     /* directory where last mounted */
//     __le32  s_algorithm_usage_bitmap; /* For compression */
//     /*
//      * Performance hints.  Directory preallocation should only
//      * happen if the EXT2_COMPAT_PREALLOC flag is on.
//      */
//     __u8    s_prealloc_blocks;  /* Nr of blocks to try to preallocate*/
//     __u8    s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
//     __u16   s_padding1;
//     /*
//      * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
//      */
//     __u8    s_journal_uuid[16]; /* uuid of journal superblock */
//     __u32   s_journal_inum;     /* inode number of journal file */
//     __u32   s_journal_dev;      /* device number of journal file */
//     __u32   s_last_orphan;      /* start of list of inodes to delete */
//     __u32   s_hash_seed[4];     /* HTREE hash seed */
//     __u8    s_def_hash_version; /* Default hash version to use */
//     __u8    s_reserved_char_pad;
//     __u16   s_reserved_word_pad;
//     __le32  s_default_mount_opts;
//     __le32  s_first_meta_bg;    /* First metablock block group */
//     __u32   s_reserved[190];    /* Padding to the end of the block */
// };















void install_ext2(void);

#endif // EXT2_H
