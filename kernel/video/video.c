#include "video.h"
#include "util/string.h"

console_t *current_console;
static size_t vprintf(console_t *console, const char *format, va_list ap);

const char *splash =    "11000011001100000000110000001100111111001100000011"
                        "11000011001100000000111100111100001100000011001100"
                        "11000011001100000000110011001100001100000000110000"
                        "11000011001100000000110000001100001100000011001100"
                        "00111100001111111100110000001100111111001100000011";

void console_init(console_t *console,
                  vmem_color_t font_color,
                  vmem_color_t background_color)
{
    console->color = font_color | (background_color << 4);
    console->pos_x = 0;
    console->pos_y = 0;
    console->vmem = (char *)VIDEO_START;

    current_console = console;
}

static void set_cursor(int x, int y)
{
    uint16_t pos = y * COLUMNS + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void splashscreen(console_t *console)
{
    int splash_lines = 5;
    int splash_cols = strlen(splash) / splash_lines;

    console_clear(console);
    for (int i = 0; i < splash_lines; i++)
    {
        for (int j = 0; j < splash_cols; j++)
        {
            if (splash[i*splash_cols + j] == '1')
                console->vmem[(i * COLUMNS + j) * 2 + 1] = BLACK;
        }
    }
}

void console_clear(console_t *console)
{
    for (size_t i = 0; i < COLUMNS*LINES*2; i+=2)
    {
        console->vmem[i] = 0;
        console->vmem[i+1] = console->color;
    }
    set_cursor(console->pos_x = 0,
               console->pos_y = 0);
}

static void scroll(console_t *console)
{
    console->pos_y = LINES - 1;
    int copy_length = (LINES - 1) * COLUMNS * 2;
    memcpy(console->vmem,
           console->vmem + COLUMNS*2,
           copy_length);

    char *last_line = console->vmem + copy_length;
    for (int i = 0; i < COLUMNS*2; i+=2)
    {
        last_line[i] = 0;
        last_line[i+1] = console->color;
    }
    set_cursor(0, LINES-1);
}

static void update(console_t *console)
{
    if (console->pos_x >= COLUMNS)
    {
        console->pos_x = 0;
        if (++console->pos_y >= LINES)
        {
            scroll(console);
            return;
        }
    }
    set_cursor(console->pos_x, console->pos_y);
}

char putchar(console_t *console, char c)
{
    if (c == '\n')
    {
        console->pos_x = 0;
        console->pos_y++;
        if (console->pos_y >= LINES)
        {
            scroll(console);
        }
    }
    else
    {
        size_t offset = console->pos_y * COLUMNS + console->pos_x++;
        console->vmem[offset*2] = c;
        console->vmem[offset*2+1] = console->color;
    }

    update(console);
    return c;
}

size_t puts(console_t *console, char *str)
{
    size_t len = 0;
    while (*str != 0)
    {
        putchar(console, *str);
        str++;
        len++;
    }
    return len;
}

static size_t vprintf(console_t *console, const char *format, va_list ap)
{
    size_t len = 0;
    char *str = (char*)format;
    char strbuf[512];
    while (*str)
    {
        if (*str == '%')
        {
            switch (*(++str))
            {
            case 'd':
                itoa(va_arg(ap, int), strbuf);
                len += puts(console, strbuf);
                break;
            case 'c':
                len += putchar(console, va_arg(ap, int));
            case 's':
                len += puts(console, va_arg(ap, char*));
                break;
            case 'x':
                itoxa(va_arg(ap, int), strbuf);
                len += puts(console, strbuf);
                break;
            default:
                str++;
            }

            str++;
            continue;
        }

        putchar(console, *str);
        len++;
        str++;
    }

    return len;
}

size_t kprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    size_t ret = vprintf(current_console, format, ap);
    va_end(ap);

    return ret;
}
