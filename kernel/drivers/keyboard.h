#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <sched/block.h>
#include <util/types.h>

#define KBD_BUF 4096

typedef enum
{
    ESCAPE, CTRL, SHIFT,
    ALT, CAPS, NUMLOCK, ROLL,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
} skeys_t;

#define SCANSET_SIZE 0x58


typedef enum
{
    KBD_BUFFER
} kbd_mode_t;

struct kbd_file_struct
{
    kbd_mode_t mode;
    unsigned char kbdbuf[KBD_BUF];
    int read_index;
    int write_index;
    blocklist_t *blocker;
};

void keyboard_setup(void);

#endif // KEYBOARD_H
