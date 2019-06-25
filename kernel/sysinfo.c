#include <sysinfo.h>
#include <debug.h>

static uint64_t flags = 0;
static char cpu_vendor_str[13];

extern int get_cpu_vendor(char *s);
extern int get_cpu_flags(uint64_t *flags);

static const char *cpu_flags[] = {
    "fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
    "cx8", "apic", "*", "sep", "mtrr", "pge", "mca", "cmov",
    "pat", "pse36", "psn", "clf", "*", "dtes", "acpi", "mmx",
    "fxsr", "sse", "sse2", "ss", "htt", "tm1", "ia64", "pbe"
};

char *cpu_vendor()
{
    get_cpu_vendor(cpu_vendor_str);
    cpu_vendor_str[12] = 0;
    return cpu_vendor_str;
}


int cpu_has(uint64_t flag)
{
    if (flags == 0)
        get_cpu_flags(&flags);

    if ((flag & flags))
        return 1;
    return 0;
}

void print_cpu_flags()
{
    get_cpu_flags(&flags);
    for (int i = 0; i < 32; i++)
    {
        if ((flags & (1 << i)))
        {
            if (i == 31)
                kprintf("%s\n", cpu_flags[i]);
            else
                kprintf("%s, ", cpu_flags[i]);
        }
    }
}
