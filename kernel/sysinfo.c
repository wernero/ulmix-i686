#include <sysinfo.h>
#include <debug.h>

static int flags_loaded = 0;
static uint32_t ft_flags_edx = 0;
static uint32_t ft_flags_ecx = 0;

static char cpu_vendor_str[13];

static const char *cpu_flags[] = {
    "fpu", "vme", "de", "pse", "tsc", "msr", "pae", "mce",
    "cx8", "apic", "*", "sep", "mtrr", "pge", "mca", "cmov",
    "pat", "pse36", "psn", "clf", "*", "dtes", "acpi", "mmx",
    "fxsr", "sse", "sse2", "ss", "htt", "tm1", "ia64", "pbe"
};

extern int get_cpu_vendor(char *str);

char *cpu_vendor()
{
    get_cpu_vendor(cpu_vendor_str);
    cpu_vendor_str[12] = 0;
    return cpu_vendor_str;
}

extern int cpuid32(
        unsigned long *eax,
        unsigned long *ebx,
        unsigned long *ecx,
        unsigned long *edx
);


int cpu_supports(int feature)
{
    if (flags_loaded == 0)
    {
        unsigned long eax = 1, ebx, ecx, edx;
        if (cpuid32(&eax, &ebx, &ecx, &edx) < 0)
            return 0;
        ft_flags_ecx = ecx;
        ft_flags_edx = edx;

        flags_loaded = 1;
    }

    if (feature < 32)  return ((ft_flags_edx & BIT(feature)));
    if (feature < 64)  return ((ft_flags_ecx & BIT(feature - 32)));

    return 0;
}

void print_cpu_flags()
{
    // just load the flags
    cpu_supports(0);

    for (int i = 0; i < 32; i++)
    {
        if ((ft_flags_edx & (1 << i)))
        {
            kprintf(" %s", cpu_flags[i]);
        }
    }
    kprintf("\n");
}
