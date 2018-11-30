#ifndef SERIAL_H
#define SERIAL_H

#define TTYS1    0x3f8
#define TTYS2    0x2f8
#define TTYS3    0x3e8
#define TTYS4    0x2e8

#include "util/types.h"

// hard coded to 38400 baud TODO: make it configurable
void serial_open(uint16_t port, uint16_t baud);
void serial_putchar(uint16_t port, char c);
void serial_write(uint16_t port, char *buf, size_t len);

#endif // SERIAL_H
