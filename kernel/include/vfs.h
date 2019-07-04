#ifndef VFS_H
#define VFS_H

#include <fs.h>
#include <types.h>
#include <devices.h>

enum ftype_e
{
    F_FIFO      = 0x1000,
    F_CHAR      = 0x2000,
    F_DIR       = 0x4000,
    F_BLOCK     = 0x6000,
    F_REGULAR   = 0x8000,
    F_SYMLINK   = 0xa000,
    F_SOCKET    = 0xc000,
    F_UNKNOWN   = 0x0000
};

struct mntp_struct
{
      void *fs_data;

      struct hd_struct *part;
      const struct fs_struct *fs_info;
};

struct direntry_struct
{
    enum ftype_e type;  // unix file type
    uint16_t mode;      // mode, permissions, etc.

    unsigned long size;     // size in bytes
    unsigned long blocks;   // size in 512B blocks

    // uid, gid should go here ...

    char name[256];
    unsigned long inode_no;

    struct dir_struct *parent;      // points to the parent inode
    struct dir_struct *directory;   // is valid if direntry_struct is a directory
    struct direntry_struct *next;   // next child on the same level

    void *payload;
};

struct dir_struct
{
    /* to load the corresponding direntry and all immediate
     * children of a directory, just set it's inode_no to
     * an arbitrary value and call the filesystem's
     * get_direntries() function. it will then load the
     * whole structure based on the inode value. */
    unsigned long inode_no;

    struct mntp_struct *fs_info;
    struct dir_struct *mnt_point;

    struct dir_struct *parent;
    struct direntry_struct *dir_entry;
    struct direntry_struct *entries;
};

int namei(const char *pathname, struct direntry_struct **node);
void get_direntries(struct dir_struct *dir);

#endif // VFS_H
