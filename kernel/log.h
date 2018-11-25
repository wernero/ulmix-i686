#ifndef LOG_H
#define LOG_H

typedef enum
{
    KLOG_INFO,
    KLOG_WARN,
    KLOG_FAILURE
} loglevel_t;

void klog(loglevel_t lvl, const char *msg, ...);

#endif // LOG_H
