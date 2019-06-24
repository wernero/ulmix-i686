#include <sysinfo.h>

static char cpu_vendor_str[13];
extern int get_cpu_vendor(char *s);

char *cpu_vendor()
{
    get_cpu_vendor(cpu_vendor_str);
    cpu_vendor_str[12] = 0;
    return cpu_vendor_str;
}
