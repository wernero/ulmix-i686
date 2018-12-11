#ifndef VIDEO_H
#define VIDEO_H

#include "util/types.h"
#include "util/util.h"

#define VIDEO_START 0xB8000
#define COLUMNS     80
#define LINES       28

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

struct tty_struct *tty_open(void);
ssize_t tty_write(struct tty_struct *tty, char *buf, int len);
void tty_putchar(struct tty_struct *tty, char c);
void tty_focus(struct tty_struct *tty);


#endif // VIDEO_H
