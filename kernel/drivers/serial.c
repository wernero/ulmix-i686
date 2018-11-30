#include "serial.h"
#include "util/util.h"

void serial_open(uint16_t port, uint16_t baud)
{
    outb(port + 1, 0x00);
    outb(port + 3, 0x80);
    outb(port + 0, 0x03);
    outb(port + 1, 0x00);
    outb(port + 3, 0x03);
    outb(port + 2, 0xC7);
    outb(port + 4, 0x0B);
}

void serial_putchar(uint16_t port, char c)
{
    while ((inb(port + 5) & 0x20) == 0);
    outb(port, c);
}

void serial_write(uint16_t port, char *buf, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        serial_putchar(port, buf[i]);
    }
}
