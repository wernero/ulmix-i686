#include <debug.h>
#include <types.h>
#include <asm.h>

static const uint16_t port = 0x3f8;

#define BAUD_115200     1
#define BAUD_57600      2
#define BAUD_38400      3
#define BAUD_28800      4
#define BAUD_23040      5
#define BAUD_19200      6
#define BAUD_14400      8
#define BAUD_12800      9
#define BAUD_11520      10
#define BAUD_9600       12
#define BAUD_7680       15

static int serial_initialized = 0;
static void serial_init(int baud)
{
    uint16_t divisor = baud;
    if (baud > 20) {
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
}

void debug_putchar(char c)
{
    if (!serial_initialized)
        serial_init(BAUD_38400);

    while ((inb(port + 5) & 0x20) == 0);
    outb(port, c);
}

void panic(const char *msg)
{

    kprintf("\n === PANIC ===\n\n"
            "%s\n\n"
            "execution halted.\n",
            msg);

    cli();
    hlt();
}
