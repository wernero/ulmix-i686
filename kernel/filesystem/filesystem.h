#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "util/util.h"

enum _whence
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef uint16_t mode_t;

// file system related System Calls:

int     sc_open     (const char *pathname, int flags);
int     sc_creat    (const char *pathname, mode_t mode);
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



void vfs_init(void);

#endif // FILESYSTEM_H
