#ifndef LOG_H
#define LOG_H

typedef enum
{
    KLOG_INFO,
    KLOG_WARN,
    KLOG_FAILURE,
    KLOG_DEBUG,
    KLOG_EXCEPTION
} loglevel_t;

void klog(loglevel_t lvl, const char *format, ...);

#endif // LOG_H
