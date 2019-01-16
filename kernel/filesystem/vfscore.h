#ifndef VFSCORE_H
#define VFSCORE_H

#include "fs_syscalls.h"
#include <util/types.h>

#define VFS_NAME_LEN 255
#define VFS_INODE_BLOCK_TABLE_LEN 128

typedef enum
{
    FIFO,
    CHARDEVICE,
    DIRECTORY,
    BLOCKDEVICE,
    REGULAR,
    SYMLINK,
    SOCKET,
    UNKOWN
} ftype_t;

struct gd_struct // group descriptor -> ext2 specific
{
    unsigned long  bg_block_bitmap;         /* Blocks bitmap block */
    unsigned long  bg_inode_bitmap;         /* Inodes bitmap block */
    unsigned long  bg_inode_table;          /* Inodes table block */
    unsigned long  bg_free_blocks_count;    /* Free blocks count */
    unsigned long  bg_free_inodes_count;    /* Free inodes count */
    unsigned long  bg_used_dirs_count;      /* Directories count */
    unsigned long  bg_pad;

    struct gd_struct *bg_next;
};

struct direntry_struct // kernel's representation of an inode
{
    ftype_t type;               // unix file type
    unsigned int  mode;         // permissions, etc.
    unsigned long size;         // size in bytes
    unsigned long size_blocks;	// block size is always assumed in 512bye

    unsigned int read_opens;    // how many times is the file open for read?
    unsigned int write_opens;   // is the file open for write? 0=no, 1=yes

    // uid, gid should go here ...

    char name[VFS_NAME_LEN];    // file's name
    unsigned long inode_no;     // inode number in the disk filesystem

    void *payload;
    struct inode_block_table *blocks;   // *TODO: what's that?

    struct dir_struct *parent;          // *TODO: parent vs. directory? what's the difference?
    struct dir_struct *directory;
    struct direntry_struct *next;
};

struct dir_struct
{
    int mountpoint;                     // is mountpoint?
    struct sb_struct *sb;               // filesystem specific superblock
    struct gendisk_struct *bd;          // block device
    struct hd_struct *partition;        // partition associated with the filesystem
    char name[VFS_NAME_LEN];            // directory name
    unsigned long inode_no;             // disk filesystem inode id

    struct dir_struct *parent;          // parent directory
    struct direntry_struct *entries;    // contained files (linked list)
};

struct inode_block_table /*** should be moved to ext2.c (filesystem specific) */
{
    // add flags for caching of this information

    struct direntry_struct  *file;                  // which file does the table belong to

    uint32_t block_count_total;                     // blocks total of file
    uint32_t block_count;                           // blocks in current struct
    uint32_t blocks[VFS_INODE_BLOCK_TABLE_LEN];     // inode blocks

    struct inode_block_table *next;                 // next block
};

struct file_struct;
struct filesystem_struct // filesystem interface
{
    char *name;
    int (*fs_probe)(struct gendisk_struct *bd, int part);
    int (*fs_mount)(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct gendisk_struct *bd, int part);
    int (*fs_get_direntry)(struct dir_struct *miss);
    int (*fs_get_inode)(struct direntry_struct *entry, unsigned long inode_no);

    // standard file system calls:
    int (*fs_read)(struct file_struct *fd, char *buf, size_t len);
    int (*fs_write)(struct file_struct *fd, char *buf, size_t len);
    ssize_t (*fs_seek)(struct file_struct *fd, size_t offset, int whence);
};

struct sb_struct /*** should be moved into ext2.c */
{
    struct filesystem_struct *fs;

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
    unsigned long s_groups_count;       /* Number of groups in the fs */
    unsigned long s_overhead_last;      /* Last calculated overhead */
    unsigned long s_blocks_last;        /* Last seen block count */

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

void vfs_init(void);

#endif // VFSCORE_H
