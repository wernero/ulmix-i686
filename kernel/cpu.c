#include "cpu.h"

uint32_t flags[2];

void setup_cpu()
{
    cpuid(CPUID_FLAGS, flags, flags+1);
}
