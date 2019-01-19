#include "tty.h"
#include <errno.h>
#include <kdebug.h>
#include <filesystem/open.h>
#include <filesystem/fs_syscalls.h>
#include <devices/devices.h>

struct file_struct *kbd = NULL;
struct tty_struct *tty1;

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

    if (varg != 1)
    {
        return -ENODEV;
    }

    fd->drv_struct = (void*)varg;
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
    else if (c == '\t')
    {
        for (int i = 0; i < TABWIDTH; i++)
        {
            tty->vmem[pos + 2*i] = 0;
            tty->vmem[pos + 2*2 + 1] = tty->color;
            tty->pos_x++;
        }
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
    int i;
    for (i = 0; i < count; i++)
    {
        _tty_putchar(tty1, buf[i]);
    }
    set_cursor(tty1);
    return i;
}

static ssize_t tty_read(struct file_struct *fd, char *buf, size_t count)
{
    if (kbd == NULL)
        return -EAGAIN;

    if (kbd->fops.read == NULL)
        return -EAGAIN;

    return kbd->fops.read(kbd, buf, count);
}

static int tty_ioctl(struct file_struct *fd, unsigned long request, unsigned long arg)
{
    (void)arg;

    if (request == IOCTL_FOCUS)
    {
        tty_focus(fd->drv_struct);
        return SUCCESS;
    }

    return -ENOSYS;
}

static int tty_release(struct file_struct *fd)
{
    return SUCCESS;
}

void tty_setup(void)
{
    struct fd_fops_struct fops =
    {
        .open = tty_open,
        .ioctl = tty_ioctl,
        .read = tty_read,
        .write = tty_write,
        .seek = NULL,
        .close = tty_release
    };
    register_cd(MAJOR_TTY, "tty", fops);

    tty1 = kmalloc(sizeof(struct tty_struct), 1, "tty_struct");
    tty1->pos_x = 0;
    tty1->pos_y = 0;
    tty1->vmem = tty1->tty_mem;
    tty1->color = WHITE | (BLACK << 4);

    clear(tty1);
    tty_focus(tty1);

    int kbdfd = open_by_major(MAJOR_KEYBOARD, 1, O_RDONLY, NULL);
    if (kbdfd < 0)
    {
        klog(KLOG_WARN, "tty: could not open keyboard (errno %d)", -kbdfd);
    }
    else
    {
        kbd = get_fd(kbdfd);
    }
}
