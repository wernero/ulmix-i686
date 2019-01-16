#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_SELECT_MODE     0x10
#define KBD_MODE_RAWBUF     0x00    // raw scancodes
#define KBD_MODE_ASCII      0x01    // keyboard in ascii mode
#define KBD_MODE_ASYNC      0x02    // not yet implemented

#define KBD_SELECT_LAYOUT   0x20
#define KBD_BILAYOUT_AT     0x00    // builtin keyboard layout: AT
#define KBD_BILAYOUT_US     0x01    // builtin keyboard layout: US

#endif // KEYBOARD_H
