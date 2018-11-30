#ifndef KEYBOARD_H
#define KEYBOARD_H

typedef enum
{
    BUFFER_MODE,
    REALTIME_MODE
} keyboard_mode_t;

void keyboard_setup(keyboard_mode_t mode);
int key_is_pressed(int key);

#endif // KEYBOARD_H
