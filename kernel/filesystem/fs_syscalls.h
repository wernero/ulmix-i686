#ifndef FS_SYSCALLS_H
#define FS_SYSCALLS_H

#include <util/util.h>
#include "vfscore.h"

typedef enum
{
    O_RDONLY    =0x00,
    O_APPEND    =0x01,
    O_CREAT     =0x02,
    O_RDWR      =0x04,
    O_WRONLY    =0x08
} openflags_t;

enum _whence
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

struct file_struct;

struct fd_fops_struct
{
    int     (*open) (struct file_struct *fd, int flags, int varg);
    ssize_t (*read) (struct file_struct *fd, char *buf, size_t len);
    ssize_t (*write)(struct file_struct *fd, char *buf, size_t len);
    ssize_t (*seek) (struct file_struct *fd, size_t offset, int whence);
    int     (*ioctl)(struct file_struct *fd, unsigned long request, unsigned long arg);
    int     (*close)(struct file_struct *fd);
};

struct file_struct // file descriptor
{
    struct direntry_struct *direntry;
    openflags_t open_mode;
    size_t seek_offset;

    void *drv_struct;

    struct fd_fops_struct fops;
};

/* file system related system calls */

int     sys_open     (char *pathname, int flags);
int     sys_creat    (const char *pathname, int mode);
ssize_t sys_write    (int fd, void *buf, size_t count);
ssize_t sys_read     (int fd, void *buf, size_t count);
int     sys_close    (int fd);
int     sys_link     (const char *oldpath, const char *newpath);
int     sys_unlink   (const char *pathname);
ssize_t sys_lseek    (int fd, size_t offset, int whence);
int     sys_ioctl    (int fd, unsigned long request, unsigned long arg);

// stat()
// mount()
// fstat()
// mkdir()
// rmdir()
// rename()

#endif // FS_SYSCALLS_H
