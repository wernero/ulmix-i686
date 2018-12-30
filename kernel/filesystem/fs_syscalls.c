#include "fs_syscalls.h"
#include "filesystem/path.h"
#include <errno.h>
#include <sched/task.h>
#include <sched/process.h>
#include <kdebug.h>

extern thread_t *current_thread;
static int insert_fd(struct file_struct *fd)
{
    process_t *p = current_thread->process;

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (p->files[i] == NULL)
        {
            p->files[i] = fd;
            return i;
        }
    }

    return -ENOBUFS;
}

int sc_open(char *pathname, int flags)
{
    struct direntry_struct *node;
    if (namei(pathname, &node) < 0)
    {
        return -ENOENT;
    }

// all stored in direntry_struct
//    struct inode_struct *inode = (struct inode_struct*)node->payload;

    // for now, don't allow to open directories
    if (node->type == DIRECTORY)
        return -EISDIR;

    // create file descriptor
    struct file_struct *fd = kmalloc(sizeof(struct file_struct), 1, "sc_open file_struct");
    fd->direntry = node;
    fd->open_mode = flags;
    fd->seek_offset = 0;

    node->fd = fd;

    if ((flags | O_WRONLY) || (flags | O_RDWR) || (flags | O_APPEND))
    {
        // MUTEX!!!
        if (node->read_opens > 0 || node->write_opens > 0)
        {
            return -EMFILE;
        }

        node->write_opens++;
    }
    else
    {
        // MUTEX!!!
        if (node->write_opens > 0)
        {
            return -EMFILE;
        }

        node->read_opens++;
    }

    return insert_fd(fd);
}

int sc_creat(const char *pathname, int mode)
{
    return -1;
}

ssize_t sc_write(int fd, void *buf, size_t count)
{
    if (fd == 912)
    {
        // debug !!!
        klog(KLOG_INFO, buf);
    }

    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;

    return fds->fops.write(fds, buf, count);			//TODO why use fops? use route via direntry->dirstruct->sb->fs...
}

ssize_t sc_read(int fd, void *buf, size_t count)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;

//    return fds->fops.read(fds, buf, count);			//TODO why use fops? use route via direntry->dirstruct->sb->fs...
    return fds->direntry->parent->sb->fs->fs_read(fds->direntry, buf, count);
}

int sc_close(int fd)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;

    //struct inode_struct *inode = (struct inode_struct*)fds->direntry->payload;

    // free resources

    return 0;
}

int sc_link(const char *oldpath, const char *newpath)
{
    return -1;
}

int sc_unlink(const char *pathname)
{
    return -1;
}

ssize_t sc_lseek(int fd, size_t offset, int whence)
{
    struct file_struct *fds = current_thread->process->files[fd];
    if (fds == NULL)
        return -EBADF;


    switch(whence)
    {
    case SEEK_SET:
        fds->seek_offset = offset;
        break;
    case SEEK_CUR:
        fds->seek_offset += offset;
        break;
    default:
        return -1;
    }

    return fds->seek_offset;
}
