#ifndef FS_H
#define FS_H

#include <fops.h>
#include <time.h>
#include <devices.h>

struct fs_struct
{
    const char *name;
    uint8_t mbr_id;

    struct fs_fops_struct fops;
};

int register_fs(const struct fs_struct *fs);
const struct fs_struct *probe_fs(struct hd_struct *part);
void gendisk_partscan(struct gendisk_struct *bd);

int do_mount(int dev_major, int dev_minor, const char *mount_path);

// === SYSCALLS ===

typedef int mode_t;
typedef long off_t;
typedef long dev_t;
typedef unsigned long ino_t;
typedef unsigned long nlink_t;
typedef unsigned long uid_t;
typedef unsigned long gid_t;
typedef unsigned long blksize_t;
typedef unsigned long blkcnt_t;

struct stat
{
    dev_t     st_dev;         /* ID of device containing file */
    ino_t     st_ino;         /* Inode number */
    mode_t    st_mode;        /* File type and mode */
    nlink_t   st_nlink;       /* Number of hard links */
    uid_t     st_uid;         /* User ID of owner */
    gid_t     st_gid;         /* Group ID of owner */
    dev_t     st_rdev;        /* Device ID (if special file) */
    off_t     st_size;        /* Total size, in bytes */
    blksize_t st_blksize;     /* Block size for filesystem I/O */
    blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

    struct timespec st_atim;  /* Time of last access */
    struct timespec st_mtim;  /* Time of last modification */
    struct timespec st_ctim;  /* Time of last status change */
};

int sys_open(const char *pathname, int flags, mode_t mode);
int sys_close(int fd);

ssize_t sys_read(int fd, void *buf, size_t count);
ssize_t sys_write(int fd, void *buf, size_t count);

off_t sys_lseek(int fd, off_t offset, int whence);

int sys_link(const char *oldpath, const char *newpath);
int sys_unlink(const char *pathname);

int sys_stat(const char *pathname, struct stat *statbuf);
int sys_fstat(int fd, struct stat *statbuf);

#endif // FS_H
