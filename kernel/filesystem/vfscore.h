#ifndef VFSCORE_H
#define VFSCORE_H

#include "fs_syscalls.h"
#include <util/types.h>

#define VFS_NAME_LEN 255

typedef enum
{
    FIFO        = 0x1000,
    CHARDEVICE  = 0x2000,
    DIRECTORY   = 0x4000,
    BLOCKDEVICE = 0x6000,
    REGULAR     = 0x8000,
    SYMLINK     = 0xa000,
    SOCKET      = 0xc000,
    UNKOWN      = 0x0000
} ftype_t;

struct filesystem_struct;
struct mntp_struct // mountpoint description
{
    void *sb;                       // filesystem-specific descriptor structure
    struct gendisk_struct *bd;      // block device containing the file system
    struct filesystem_struct *fs;   // fs file operations
};

struct direntry_struct // kernel's representation of an inode
{
    ftype_t type;               // unix file type
    uint16_t  mode;             // mode, permissions, etc.
    unsigned long size;         // size in bytes
    unsigned long size_blocks;	// block size is always assumed in 512bye

    unsigned int read_opens;    // how many times is the file open for read?
    unsigned int write_opens;   // is the file open for write? 0=no, 1=yes

    // uid, gid should go here ...

    char name[VFS_NAME_LEN];    // file's name
    unsigned long inode_no;     // inode number in the disk filesystem

    void *payload;
    uint32_t bptr1;
    struct inode_block_table *blocks;   // *TODO: what's that?

    struct dir_struct *parent;          // *TODO: parent vs. directory? what's the difference?
    struct dir_struct *directory;
    struct direntry_struct *next;
};

struct dir_struct
{
    struct mntp_struct *mnt_info;       // mount information for the file system
    struct dir_struct *mountpoint;      // pointer to the mountpoint directory
    char name[VFS_NAME_LEN];            // directory name
    unsigned long inode_no;             // disk filesystem inode id

    struct dir_struct *parent;          // parent directory
    struct direntry_struct *entries;    // contained files (linked list)
};


struct file_struct;
struct filesystem_struct // filesystem interface
{
    char *name;
    int (*fs_probe)(struct file_struct *fd);
    int (*fs_mount)(struct filesystem_struct *fs, struct dir_struct *mountpoint, struct file_struct *fd);
    int (*fs_get_direntry)(struct dir_struct *miss);
    int (*fs_get_inode)(struct direntry_struct *entry);

    // standard file system calls:
    int (*fs_read)(struct file_struct *fd, char *buf, size_t len);
    int (*fs_write)(struct file_struct *fd, char *buf, size_t len);
    ssize_t (*fs_seek)(struct file_struct *fd, size_t offset, int whence);
};

void vfs_init(void);

#endif // VFSCORE_H
