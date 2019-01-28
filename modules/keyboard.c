#include <kdebug.h>

void module_init(void)
{
    klog(KLOG_INFO, "module init");
}

void module_cleanup(void)
{
    klog(KLOG_INFO, "module cleanup");
}
