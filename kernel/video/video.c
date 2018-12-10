#include "video.h"
#include "util/string.h"
#include "memory/kheap.h"

static void clear(struct tty_struct *tty);
static void scroll(struct tty_struct *tty);

void tty_focus(struct tty_struct *tty)
{
    tty->vmem = (char*)VIDEO_START;
    memcpy(tty->vmem, tty->tty_mem, LINES*COLUMNS*2);
}

struct tty_struct *tty_open(void)
{
    struct tty_struct *tty = kmalloc(sizeof(struct tty_struct), 1, "tty_struct");
    tty->pos_x = 0;
    tty->pos_y = 0;
    tty->vmem = tty->tty_mem;
    tty->color = WHITE | (BLACK << 4);

    clear(tty);
    return tty;
}

ssize_t tty_write(struct tty_struct *tty, char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        tty_putchar(tty, buf[i]);
    }
    return i;
}

static void scroll(struct tty_struct *tty)
{
    int src = COLUMNS * 2;
    int len = COLUMNS * (LINES - 1) * 2;
    memmove(tty->vmem, (void*)(tty->vmem + src), len);

    for (int i = len; i < COLUMNS*LINES*2; i += 2)
    {
        tty->vmem[i] = 0;
        tty->vmem[i+1] = tty->color;
    }

    tty->pos_y = LINES - 1;
}

void tty_putchar(struct tty_struct *tty, char c)
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

static void clear(struct tty_struct *tty)
{
    for (int i = 0; i < (LINES*COLUMNS*2); i += 2)
    {
        tty->vmem[i] = 0;
        tty->vmem[i+1] = tty->color;
    }

    tty->pos_x = 0;
    tty->pos_y = 0;
}

