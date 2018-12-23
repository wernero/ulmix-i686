#ifndef LOG_H
#define LOG_H

#include "util/types.h"

#define OUT_SERIAL  0x01
#define OUT_TTY     0x02

typedef enum
{
    KLOG_INFO,          // display on assoc tty
    KLOG_WARN,          // kernel log only
    KLOG_FAILURE,       // display on assoc tty
    KLOG_DEBUG,         // kernel log when debugging enabled
    KLOG_EXCEPTION,     // kernel log only
    KLOG_PANIC          // display on all ttys
} loglevel_t;


void klog(loglevel_t lvl, const char *format, ...);

void hexdump(const void* data, size_t size);

#endif // LOG_H
