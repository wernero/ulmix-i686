#include "serial.h"
#include "util/util.h"

static int serial1_enabled = 0;

void serial_open(uint16_t port, uint16_t baud)
{
    uint16_t divisor = baud;
    if (baud > 20)
    {
        divisor = 115200 / baud;
    }

    outb(port + 1, 0x00);       // disable interrupts
    outb(port + 3, 0x80);       // enable baud divisor
    outb(port + 0, divisor);    // divisor low
    outb(port + 1, 0x00);       // divisor high
    outb(port + 3, 0x03);       // 8 bits, no parity, 1 stop bit
    outb(port + 2, 0xC7);       // ?
    outb(port + 4, 0x0B);       // ?
    outb(port + 1, 0x01);       // Raise interrupt when data available

    serial1_enabled = 1;
}

void serial_putchar(uint16_t port, char c)
{
    if (serial1_enabled)
    {
        while ((inb(port + 5) & 0x20) == 0);
        outb(port, c);
    }
}

void serial_write(uint16_t port, char *buf, size_t len)
{
    if (serial1_enabled)
    {
        for (int i = 0; i < len; i++)
        {
            serial_putchar(port, buf[i]);
        }
    }
}
