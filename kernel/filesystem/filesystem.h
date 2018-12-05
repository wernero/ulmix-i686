#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"
#include "filesystem/path.h"

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

#define PERM_USER_READ  0x8000
#define PERM_USER_WRITE 0x4000
#define PERM_USER_EXEC  0x2000
#define PERM_GRP_READ   0x1000
#define PERM_GRP_WRITE  0x0800
#define PERM_GRP_EXEC   0x0400
#define PERM_OTH_READ   0x0200
#define PERM_OTH_WRITE  0x0100
#define PERM_OTH_EXEC   0x0080

typedef struct
{
    file_type_t     type;       // special file type
    size_t          io_size;    // 1 = character device, block size for block devices
    void*           drv_struct; // points to a driver-internal data structure that describes the device

    ssize_t (*read) (void* drv_struct, char *buf, size_t count);
    ssize_t (*write)(void* drv_struct, char *buf, size_t count);
    ssize_t (*seek) (void* drv_struct, size_t offset, int whence);

    uint16_t permissions;

    uid_t owner;
    gid_t group;

    char name[64];
} file_t;

typedef struct _fnode fnode_t;
struct _fnode
{
    file_t meta;
    fnode_t *parent;
    fnode_t *previous;
    fnode_t *next;
    fnode_t *children;
};

typedef struct
{
    file_t *file;
    size_t seek_offset;
} fd_t;


fnode_t *mknod(fnode_t *directory, file_t file);
fnode_t *mkdir(fnode_t *directory, char *name);
int mount(fnode_t *mount_point, file_t *device);

void vfs_init(void);

#endif // FILESYSTEM_H
