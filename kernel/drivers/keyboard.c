#include "keyboard.h"
#include "interrupts.h"
#include "util/util.h"
#include "util/types.h"

#include "video/video.h"

#define KBD_INTERRUPT   0x21

static void int_handler(void);
static keyboard_mode_t kbd_mode;

void keyboard_setup(keyboard_mode_t mode)
{
    kbd_mode = mode;
    irq_install_handler(KBD_INTERRUPT, int_handler);
}

static void int_handler()
{
    uint8_t scancode = 0;
    while (inb(0x64)&1)
    {
        if (inb(0x64)&1)
            scancode = inb(0x60);

        kprintf("scancode=%d\n", scancode);
    }
}
