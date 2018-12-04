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

typedef struct
{
    int pos_x;
    int pos_y;
    char *vmem;
    uint8_t color;
} console_t;

void console_clear(console_t *console);
void console_init(console_t *console,
                  vmem_color_t font_color,
                  vmem_color_t background_color);
void splashscreen(console_t *console);
size_t puts(console_t *console, char *str);
char putchar(console_t *console, char c);

size_t kprintf(const char *format, ...);

#endif // VIDEO_H
