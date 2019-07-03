#include <types.h>

#define VIDEO_BASE  ((void*)0xb8000)

#include <string.h>

#define LINES   25
#define COLUMNS  80

#define T_BLACK         0x0
#define T_BLUE          0x1
#define T_GREEN         0x2
#define T_CYAN          0x3
#define T_RED           0x4
#define T_PURPLE        0x5
#define T_BROWN         0x6
#define T_GRAY          0x7
#define T_DARK_GRAY     0x8
#define T_LIGHT_BLUE    0x9
#define T_LIGHT_GREEN   0xa
#define T_LIGHT_CYAN    0xb
#define T_LIGHT_RED     0xc
#define T_LIGHT_PURPLE  0xd
#define T_YELLOW        0xe
#define T_WHITE         0xf

static unsigned char current_color;
static unsigned text_x = 0;
static unsigned text_y = 0;

static unsigned char get_color(int text, int background)
{
    return (unsigned char)(text | (background << 4));
}

static void text_clear()
{
    text_x = text_y = 0;

    unsigned char *sc_ptr = VIDEO_BASE;
    unsigned char default_color = get_color(T_WHITE, T_BLACK);
    for (int x = 0; x < COLUMNS; x++)
    {
        for (int y = 0; y < LINES; y++)
        {
            *sc_ptr++ = 0;
            *sc_ptr++ = default_color;
        }
    }
}

static void text_putc(char c)
{
    int tmp;
    unsigned char *mptr = VIDEO_BASE;
    mptr += ((text_y * COLUMNS + text_x) * 2);
    switch (c)
    {
    case '\b':
        *mptr-- = 0;
        *mptr = current_color;
        text_x--;
        break;
    case '\n':
        text_x = 0;
        if (text_y == LINES - 1)
        {
            memcpy(VIDEO_BASE, VIDEO_BASE + (COLUMNS*2), (LINES-1)*COLUMNS*2);
            mptr = VIDEO_BASE + (LINES-1)*COLUMNS*2;
            for (tmp = 0; tmp < COLUMNS*2; tmp += 2)
            {
                mptr[tmp] = 0;
                mptr[tmp+1] = current_color;
            }
        }
        else
        {
            text_y++;
        }
        break;
    case '\r':
        text_x = 0;
        break;
    case '\t':
        tmp = 4;
        while (tmp--)
            text_putc(0);
        break;
    default:
        if (text_x == COLUMNS)
        {
            text_putc('\n');
            text_putc(c);
        }
        else
        {
        *mptr++ = c;
        *mptr = current_color;
        text_x++;
        }
    }
}

static int setup = 0;
void __init init_vga()
{
    if (setup)
        return;
    text_clear();
    setup = 1;
    current_color = get_color(T_CYAN, T_BLACK);
    // Get display mode ?
}

ssize_t vga_write(void *buf, size_t len)
{
    if (!setup)
        init_vga();

    size_t i = len;
    char *buf_ptr = buf;
    while (i--)
        text_putc(*buf_ptr++);
    return len;
}
