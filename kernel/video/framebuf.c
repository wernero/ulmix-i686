#include "framebuf.h"

static vmode_t current_vmode = TEXT_80x28;

vmode_t get_video_mode(void)
{
    return current_vmode;
}
