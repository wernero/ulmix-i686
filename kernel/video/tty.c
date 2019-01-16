#include "tty.h"
#include <errno.h>
#include <kdebug.h>
#include <filesystem/fs_syscalls.h>
#include <drivers/devices.h>

static struct file_struct *ttyk = NULL;
struct file_struct *keyboard;


static void set_cursor(struct tty_struct *tty)
{
    if (tty->vmem == tty->tty_mem)
        return;

    int pos = tty->pos_y * COLUMNS + tty->pos_x;

    outb(0x3d4, 0x0f);
    outb(0x3d5, (unsigned char)(pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (unsigned char)((pos >> 8) & 0xff));
}

static void tty_focus(struct tty_struct *tty)
{
    tty->vmem = (char*)VIDEO_START;
    memcpy(tty->vmem, tty->tty_mem, LINES*COLUMNS*2);
    set_cursor(tty);
}

static void clear(struct tty_struct *tty)
{
    for (int i = 0; i < (LINES*COLUMNS*2); i += 2)
    {
        tty->vmem[i] = 0;
        tty->vmem[i+1] = tty->color;
    }

    tty->pos_x = 0;
    tty->pos_y = 0;
    set_cursor(tty);
}

static int tty_open(struct file_struct *fd, int flags, int varg)
{
    (void)flags;
    (void)varg;

    struct tty_struct *tty = kmalloc(sizeof(struct tty_struct), 1, "tty_struct");
    tty->pos_x = 0;
    tty->pos_y = 0;
    tty->vmem = tty->tty_mem;
    tty->color = WHITE | (BLACK << 4);

    clear(tty);
    tty_focus(tty);
    fd->drv_struct = tty;
    return SUCCESS;
}

static void scroll(struct tty_struct *tty)
{
    int src = COLUMNS * 2;
    int len = COLUMNS * (LINES - 1) * 2;
    memcpy(tty->vmem, (void*)(tty->vmem + src), len);

    for (int i = len; i < COLUMNS*LINES*2; i += 2)
    {
        tty->vmem[i] = 0;
        tty->vmem[i+1] = tty->color;
    }

    tty->pos_y = LINES - 1;
    set_cursor(tty);
}


static void _tty_putchar(struct tty_struct *tty, char c)
{
    int pos = (tty->pos_y * COLUMNS + tty->pos_x) * 2;

    if (c == '\n')
    {
        for (int i = pos; tty->pos_x < COLUMNS; i += 2)
        {
            tty->vmem[i] = 0;
            tty->vmem[i+1] = tty->color;
            tty->pos_x++;
        }

        tty->pos_x = 0;
        tty->pos_y++;
    }
    else if (c == '\b')
    {
        tty->vmem[pos - 2] = 0;
        tty->vmem[pos - 1] = tty->color;
        tty->pos_x--;
    }
    else
    {
        tty->vmem[pos] = c;
        tty->vmem[pos + 1] = tty->color;
        tty->pos_x++;
    }

    if (tty->pos_x >= COLUMNS)
    {
        tty->pos_y++;
        tty->pos_x = 0;
    }

    if (tty->pos_y >= LINES)
    {
        scroll(tty);
    }
}

static ssize_t tty_write(struct file_struct *fd, char *buf, size_t count)
{
    struct tty_struct *tty = (struct tty_struct*)fd->drv_struct;
    int i;
    for (i = 0; i < count; i++)
    {
        _tty_putchar(tty, buf[i]);
    }
    set_cursor(tty);
    return i;
}

static ssize_t tty_read(struct file_struct *fd, char *buf, size_t count)
{
    if (keyboard == NULL)
        return -EAGAIN;

    // we should have a per-device keyboard buffer
    return -ENOSYS;
}

static ssize_t tty_seek(struct file_struct *fd, size_t offset, int whence)
{
    return -ENOSYS;
}

static int tty_ioctl(struct file_struct *fd, unsigned long request, unsigned long arg)
{
    if (request == IOCTL_FOCUS)
    {
        tty_focus(fd->drv_struct);
        return SUCCESS;
    }

    return -ENOSYS;
}

static int tty_release(struct file_struct *fd)
{
    return -ENOSYS;
}

void tty_setup(void)
{
    struct fd_fops_struct fops =
    {
        .open = tty_open,
        .ioctl = tty_ioctl,
        .read = tty_read,
        .write = tty_write,
        .seek = tty_seek,
        .close = tty_release
    };
    register_cd(MAJOR_TTY, "tty", fops);

    /*if (open_chardev(&keyboard, MAJOR_KEYBOARD, O_RDONLY) < 0)
    {
        klog(KLOG_WARN, "tty_setup(): failed to open keyboard");
        keyboard = NULL;
    }*/
}

static int ttyk_init(void)
{
    ttyk = kmalloc(sizeof(struct file_struct), 1, "file_struct ttyk");
    return tty_open(ttyk, 0, 0);
}

static void tty_putchar(struct tty_struct *tty, char c)
{
    _tty_putchar(tty, c);
    set_cursor(tty);
}

ssize_t tty_kernel_putchar(char c)
{
    if (ttyk == NULL)
    {
        if (ttyk_init() < 0)
        {
            return -1;
        }
    }

    tty_putchar((struct tty_struct*)ttyk->drv_struct, c);
    return SUCCESS;
}

ssize_t tty_kernel_write(char *buf, size_t count)
{
    if (ttyk == NULL)
    {
        if (ttyk_init() < 0)
        {
            return -1;
        }
    }

    return tty_write(ttyk, buf, count);
}
