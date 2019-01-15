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
    ssize_t (*read) (struct file_struct *fd, char *buf, size_t len);
    ssize_t (*write)(struct file_struct *fd, char *buf, size_t len);
    ssize_t (*seek) (struct file_struct *fd, size_t offset, int whence);
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


int open_chardev(struct file_struct *fds, int major, int flags);

/* file system related system calls */

int     sc_open     (char *pathname, int flags);
int     sc_creat    (const char *pathname, int mode);
ssize_t sc_write    (int fd, void *buf, size_t count);
ssize_t sc_read     (int fd, void *buf, size_t count);
int     sc_close    (int fd);
int     sc_link     (const char *oldpath, const char *newpath);
int     sc_unlink   (const char *pathname);
ssize_t sc_lseek    (int fd, size_t offset, int whence);

// stat()
// mount()
// fstat()
// mkdir()
// rmdir()
// rename()

#endif // FS_SYSCALLS_H
