#include "keyboard.h"
#include "devices.h"
#include <util/util.h>
#include <interrupts.h>
#include <kdebug.h>
#include <errno.h>

#define KBD_INTERRUPT   0x21
#define KBD_DATA        0x60
#define KBD_COMMAND     0x64
#define KBD_STATUS      0x64

#define MAX_OPENS       10

void int_handler(void);

static void kbdbuf_put(unsigned char c);

static blocklist_t *kbd_block = NULL;
static struct kbd_file_struct *drvp[MAX_OPENS];

static int open(struct file_struct *fd, int flags, int varg);
static int release(struct file_struct *fd);
static int ioctl(struct file_struct *fd, unsigned long request, unsigned long arg);
static ssize_t write(struct file_struct *fd, char *buf, size_t len);
static ssize_t read(struct file_struct *fd, char *buf, size_t len);
static ssize_t seek(struct file_struct *fd, size_t offset, int whence);


char kbd_at[2][SCANSET_SIZE] = {
    {
        0, ESCAPE,
        '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        'S', '\'', '\b', '\t',
        'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', 'u',
        '+', '\n', CTRL,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'o', 'a', '^',
        SHIFT, '#', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',
        SHIFT, '*', ALT, ' ', CAPS,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
        NUMLOCK, ROLL, '7', '8', '9', '-',
        '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, '<',
        F11, F12
    },
    {
        0, ESCAPE,
        '!', '"', '?', '$', '%', '&', '/', '(', ')', '=',
        '?', '\'', '\b', '\t',
        'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', 'U',
        '*', '\n', CTRL,
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'O', 'A', '^',
        SHIFT, '\'', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ';', ':', '_',
        SHIFT, '*', ALT, ' ', CAPS,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
        NUMLOCK, ROLL, '7', '8', '9', '-',
        '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, '<',
        F11, F12
    }
};

void keyboard_setup(void)
{
    klog(KLOG_DEBUG, "keyboard_setup()");
    for (int i = 0; i < MAX_OPENS; i++)
        drvp[i] = NULL;

    kbd_block = blocker();

    // register character device
    struct fd_fops_struct fops =
    {
        .open = open,
        .close = release,
        .ioctl = ioctl,
        .read = read,
        .write = write,
        .seek = seek
    };
    register_cd(MAJOR_KEYBOARD, "kbd", fops);

    // install irq handler
    irq_install_handler(KBD_INTERRUPT, int_handler);
}

void int_handler()
{
    uint8_t scancode;
    while (inb(KBD_STATUS) & 1)
    {
        scancode = inb(KBD_DATA);
        kbdbuf_put(scancode);
    }
}

static void kbdbuf_put(unsigned char c)
{
    if (c == 0)
        return;

    for (int i = 0; i < MAX_OPENS; i++)
    {
        if (drvp[i] == NULL)
            continue;

        drvp[i]->kbdbuf[drvp[i]->write_index++] = c;
        if (drvp[i]->write_index >= KBD_BUF)
            drvp[i]->write_index = 0;
    }

    // unblock threads
    blocklist_unblock(kbd_block);
}

static int open(struct file_struct *fd, int flags, int varg)
{
    struct kbd_file_struct *kdesc = kmalloc(sizeof(struct kbd_file_struct), 1, "keyboard descriptor");
    kdesc->mode = KBD_MODE_ASCII;
    kdesc->read_index = 0;
    kdesc->write_index = 0;
    kdesc->blocker = kbd_block;
    fd->drv_struct = (void*)kdesc;
    for (int i = 0; i < MAX_OPENS; i++)
    {
        if (drvp[i] == NULL)
        {
            drvp[i] = kdesc;
            return SUCCESS;
        }
    }
    return -EMFILE;
}

static int release(struct file_struct *fd)
{
    kfree(fd->drv_struct);
    for (int i = 0; i < MAX_OPENS; i++)
    {
        if (drvp[i] == fd->drv_struct)
            drvp[i] = NULL;
    }
    return SUCCESS;
}

static int set_mode(struct kbd_file_struct *kbd, unsigned long mode)
{
    switch (mode)
    {
    case KBD_MODE_ASCII:
    case KBD_MODE_RAWBUF:
        kbd->mode = mode;
        return SUCCESS;
    case KBD_MODE_ASYNC:
        return -ENOSYS;
    default:
        return -EINVAL;
    }
}

static int ioctl(struct file_struct *fd, unsigned long request, unsigned long arg)
{
    klog(KLOG_INFO, "keyboard ioctl(): request=%d, arg=%d", request, arg);
    ((struct kbd_file_struct*)fd->drv_struct)->mode = request;

    struct kbd_file_struct *kbd = (struct kbd_file_struct*)fd->drv_struct;

    switch (request)
    {
    case KBD_SELECT_MODE:
        return set_mode(kbd, arg);
    default:
        return -EINVAL;
    }

    return SUCCESS;
}

static ssize_t write(struct file_struct *fd, char *buf, size_t len)
{
    return -ENOSYS;
}

static unsigned char getcode(struct kbd_file_struct *kbd)
{
    unsigned char c = kbd->kbdbuf[kbd->read_index];
    if (c == 0)
    {
        return 0;
    }
    else
    {
        if (++kbd->read_index >= KBD_BUF)
            kbd->read_index = 0;

        if (kbd->mode == KBD_MODE_ASCII)
        {
            if (c >= SCANSET_SIZE)
                return 0;
            return kbd_at[0][c];
        }
        else if (kbd->mode == KBD_MODE_RAWBUF)
        {
            return c;
        }
        return c;
    }
}

static ssize_t read(struct file_struct *fd, char *buf, size_t len)
{
    struct kbd_file_struct *kbdfile = (struct kbd_file_struct*)fd->drv_struct;

    unsigned c;
    size_t read_count = 0;
    while (len)
    {
        c = getcode(kbdfile);

        if (c == 0)
        {
            if (read_count == 0)
            {
                // block until data gets available
                blocklist_add(kbdfile->blocker);
                continue;
            }
            else
            {
                break;
            }
        }

        *(buf++) = c;
        read_count++;
        len--;
    }

    return read_count;
}

static ssize_t seek(struct file_struct *fd, size_t offset, int whence)
{
    return -ENOSYS;
}
