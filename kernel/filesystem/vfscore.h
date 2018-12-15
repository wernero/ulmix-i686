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

struct sb_struct
{
    // TODO: to be filled in by Rainer
};

struct inode_struct
{
    // TODO: fill in
};

struct dir_struct
{
    int mountpoint;
    struct sb_struct *sb;
    char name[256];
    unsigned long inode_no;

    struct direntry_struct *entries;
};

void vfs_init(void);

#endif // VFSCORE_H
