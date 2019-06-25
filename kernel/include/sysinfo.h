#ifndef SYSINFO_H
#define SYSINFO_H

#include <types.h>

#define CPU_FPU     BIT(0)  // on chip FPU
#define CPU_VME     BIT(1)  // virtual 8086 extensions
#define CPU_DE      BIT(2)  // debugging extensions
#define CPU_PSE     BIT(3)  // page size extension (4MB pages)
#define CPU_TSC     BIT(4)  // RDTSC instruction support
#define CPU_MSR     BIT(5)  // model specific registers
#define CPU_PAE     BIT(6)  // physical address extension
#define CPU_MCE     BIT(7)  // machine check exception
#define CPU_CX8     BIT(8)  // compare and exchange 8B (CMPXCHG8)
#define CPU_APIC    BIT(9)  // on chip APIC
#define CPU_SEP     BIT(11) // fast system call (SYSENTER, SYSEXIT)
#define CPU_MTRR    BIT(12) // memory type range registers
#define CPU_PGE     BIT(13) // page global enable
#define CPU_MCA     BIT(14) // machine check architecture
#define CPU_CMOV    BIT(15) // conditional move instruction
#define CPU_PAT     BIT(16) // page attribute table support
#define CPU_PSE36   BIT(17) // 36bit page size extension
#define CPU_PSN     BIT(18) // processor serial number
#define CPU_CLF     BIT(19) // CLFLUSH instruction
#define CPU_DTES    BIT(21) // debug store
#define CPU_ACPI    BIT(22) // ACPI (thermal monitor)
#define CPU_MMX     BIT(23) // MMX instruction set
#define CPU_FXSR    BIT(24) // FXSAVE, FXSTOR
#define CPU_SSE     BIT(25) // Streaming SIMD
#define CPU_SSE2    BIT(26) // Streaming SIMD 2
#define CPU_SS      BIT(27) // self snoop
#define CPU_HTT     BIT(28) // multithreading support
#define CPU_TM1     BIT(29) // thermal monitor
#define CPU_IA64    BIT(30) // actually reserved (!)
#define CPU_PBE     BIT(31) // pending break enable

char *cpu_vendor();
int cpu_has(uint64_t flag);
void print_cpu_flags();

#endif // SYSINFO_H
