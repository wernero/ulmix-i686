#ifndef TTY_H
#define TTY_H

#include <util/util.h>
#include "framebuf.h"

typedef enum
{
    BLACK, BLUE, GREEN, CYAN,
    RED, PURPLE, BROWN, GRAY,
    DARK_GRAY, LIGHT_BLUE, LIGHT_GREEN,
    LIGHT_CYAN, LIGHT_RED,
    LIGHT_PURPLE, YELLOW, WHITE

} vmem_color_t;

struct tty_struct
{
    int pos_x;
    int pos_y;
    char tty_mem[LINES*COLUMNS*2];
    char *vmem;
    uint8_t color;
};

#define IOCTL_FOCUS 0x10

void tty_setup(void);
struct tty_struct *getty(void);
ssize_t tty_kernel_putchar(char c);
ssize_t tty_kernel_write(char *buf, size_t count);

#endif // TTY_H
