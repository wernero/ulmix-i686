#ifndef FS_SYSCALLS_H
#define FS_SYSCALLS_H

#include "util/util.h"
#include "filesystem/vfscore.h"

struct file_struct // file descriptor
{
    struct direntry_struct *direntry;

    // fops, ...
};

enum _whence
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

/* file system related system calls */

int     sc_open     (const char *pathname, int flags);
int     sc_creat    (const char *pathname, int mode);
ssize_t sc_write    (int fd, const void *buf, size_t count);
ssize_t sc_read     (int fd, const void *buf, size_t count);
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
