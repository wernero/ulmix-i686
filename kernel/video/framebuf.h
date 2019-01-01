#ifndef FRAMEBUF_H
#define FRAMEBUF_H

#define COLUMNS     80
#define LINES       28
#define VIDEO_START 0xb8000

#define VMODE get_video_mode()

typedef enum
{
    TEXT_80x28
} vmode_t;

vmode_t get_video_mode(void);

#endif // FRAMEBUF_H
