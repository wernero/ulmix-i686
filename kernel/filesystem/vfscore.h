#ifndef VFSCORE_H
#define VFSCORE_H

typedef enum
{
    FIFO,
    CHARDEVICE,
    DIRECTORY,
    BLOCKDEVICE,
    REGULAR,
    SYMLINK,
    SOCKET
} ftype_t;

struct direntry_struct
{
    ftype_t type;
    char name[256];
    unsigned long inode_no;
    void *payload;

    struct direntry_struct *next;
};

struct gd_struct
{
    unsigned long  bg_block_bitmap;        /* Blocks bitmap block */
    unsigned long  bg_inode_bitmap;        /* Inodes bitmap block */
    unsigned long  bg_inode_table;     /* Inodes table block */
    unsigned long  bg_free_blocks_count;   /* Free blocks count */
    unsigned long  bg_free_inodes_count;   /* Free inodes count */
    unsigned long  bg_used_dirs_count; /* Directories count */
    unsigned long  bg_pad;

    struct gd_struct *bg_next;
};

struct sb_struct
{

    unsigned long s_inodes_total;
    unsigned long s_blocks_total;

    unsigned long s_frag_size;  /* Size of a fragment in bytes */
    unsigned long s_frags_per_block;/* Number of fragments per block */
    unsigned long s_inodes_per_block;/* Number of inodes per block */
    unsigned long s_frags_per_group;/* Number of fragments in a group */
    unsigned long s_blocks_per_group;/* Number of blocks in a group */
    unsigned long s_inodes_per_group;/* Number of inodes in a group */
    unsigned long s_itb_per_group;  /* Number of inode table blocks per group */
    unsigned long s_gdb_count;  /* Number of group descriptor blocks */
    unsigned long s_desc_per_block; /* Number of group descriptors per block */
    unsigned long s_groups_count;   /* Number of groups in the fs */
    unsigned long s_overhead_last;  /* Last calculated overhead */
    unsigned long s_blocks_last;    /* Last seen block count */

    struct gd_struct *s_group_desc;

    void * s_es; /* Pointer to the super block in the buffer */

    // struct buffer_head * s_sbh; /* Buffer containing the super block */
    // struct ext2_super_block * s_es; /* Pointer to the super block in the buffer */
    // struct buffer_head ** s_group_desc;
    // unsigned long  s_mount_opt;
    // unsigned long s_sb_block;
    // unsigned short s_mount_state;
    // unsigned short s_pad;
    // int s_addr_per_block_bits;
    // int s_desc_per_block_bits;
    // int s_inode_size;
    // int s_first_ino;
    // spinlock_t s_next_gen_lock;
    // u32 s_next_generation;
    // unsigned long s_dir_count;
    // u8 *s_debts;
    // struct percpu_counter s_freeblocks_counter;
    // struct percpu_counter s_freeinodes_counter;
    // struct percpu_counter s_dirs_counter;
    // struct blockgroup_lock *s_blockgroup_lock;
    // /* root of the per fs reservation window tree */
    // spinlock_t s_rsv_window_lock;
    // struct rb_root s_rsv_window_root;
    // struct ext2_reserve_window_node s_rsv_window_head;
    // spinlock_t s_lock;
    // struct mb_cache *s_ea_block_cache;
    // struct dax_device *s_daxdev;
};

struct inode_struct
{
    // TODO: fill in
};

struct dir_struct
{
    int mountpoint;
    struct sb_struct *sb;
    struct gendisk_struct *bd;
    struct hd_struct *partition;
    char name[256];
    unsigned long inode_no;

    struct direntry_struct *entries;
};

void vfs_init(void);

#endif // VFSCORE_H
