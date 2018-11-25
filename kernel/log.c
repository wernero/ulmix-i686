#include "log.h"
#include "video/video.h"

void klog(loglevel_t lvl, const char *msg, ...)
{
    char *lvl_str;
    switch (lvl)
    {
    case KLOG_INFO:
        lvl_str = " INFO ";
        break;
    case KLOG_WARN:
        lvl_str = " WARN ";
        break;
    case KLOG_FAILURE:
        lvl_str = "FAILED";
        break;
    default:
        lvl_str = "  ??  ";
    }


    kprintf("[%s]: ", lvl_str);

    va_list va;
    va_start(va, msg);
    kprintf(msg, va);
    va_end(va);

    kprintf("\n");
}
