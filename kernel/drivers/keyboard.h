/*
 * ULMIX PS/2 keyboard driver
 * Copyright (C) 2018-2019 The Ulmix Operating System
 * Written by Alexander Ulmer
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <sched/sync.h>
#include <util/types.h>

#define KBD_SELECT_MODE     0x10
#define KBD_MODE_RAWBUF     0x00    // raw scancodes
#define KBD_MODE_ASCII      0x01    // keyboard in ascii mode
#define KBD_MODE_ASYNC      0x02    // not yet implemented

#define KBD_SELECT_LAYOUT   0x20
#define KBD_BILAYOUT_AT     0x00    // builtin keyboard layout: AT
#define KBD_BILAYOUT_US     0x01    // builtin keyboard layout: US

#define KBD_BUF 4096

typedef enum
{
    ESCAPE, CTRL, SHIFT,
    ALT, CAPS, NUMLOCK, ROLL,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
} skeys_t;

#define SCANSET_SIZE 0x58

struct kbd_file_struct
{
    unsigned long mode;
    unsigned char kbdbuf[KBD_BUF];
    int read_index;
    int write_index;
    blocklist_t *blocker;
};

void keyboard_setup(void);

#endif // KEYBOARD_H
