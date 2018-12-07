#ifndef VFSCORE_H
#define VFSCORE_H

/*
 * core source code for the virtual file system model
 */

#include "util/util.h"

#define PERM_USER_READ  0x8000
#define PERM_USER_WRITE 0x4000
#define PERM_USER_EXEC  0x2000
#define PERM_GRP_READ   0x1000
#define PERM_GRP_WRITE  0x0800
#define PERM_GRP_EXEC   0x0400
#define PERM_OTH_READ   0x0200
#define PERM_OTH_WRITE  0x0100
#define PERM_OTH_EXEC   0x0080


typedef enum
{
    FILE_DIRECTORY,
    FILE_DEVICE,
    FILE_FIFO,
    FILE_REAL
} file_type_t;

enum _whence
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef struct _fdesc fd_t;
typedef struct _nfile file_t;
struct _nfile
{
    file_type_t     type;       // special file type
    size_t          io_size;    // 1 = character device, block size for block devices
    void*           drv_struct; // points to a driver-internal data structure
                                // that describes the device and can be used as an
                                // identifier to the driver

    fd_t *(*open)(file_t *file);
    int (*close)(fd_t *fd);

    size_t size;

    uint16_t permissions;
    uid_t owner;
    gid_t group;

    char name[64];
};

typedef struct _fnode fnode_t;
struct _fnode
{
    file_t   meta;
    fnode_t *parent;
    fnode_t *previous;
    fnode_t *next;
    fnode_t *children;
};

struct _fdesc
{
    file_t *file;
    size_t seek_offset;
    void* fdi_struct; // points to a driver-internal data structure
                      // that describes the device and can be used as an
                      // identifier to the driver

    ssize_t (*read) (fd_t *fd, char *buf, size_t count);
    ssize_t (*write)(fd_t *fd, char *buf, size_t count);
    ssize_t (*seek) (fd_t *fd, size_t offset, int whence);
};


void vfs_init(void);
fnode_t *mknod(fnode_t *directory, file_t file);
fnode_t *mkdir(fnode_t *directory, char *name);
int insert_child_node(fnode_t *parent, fnode_t *child);

#endif // VFSCORE_H
