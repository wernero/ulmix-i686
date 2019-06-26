#ifndef SYSINFO_H
#define SYSINFO_H

#include <types.h>

#define CPU_FPU     0  // on chip FPU
#define CPU_VME     1  // virtual 8086 extensions
#define CPU_DE      2  // debugging extensions
#define CPU_PSE     3  // page size extension (4MB pages)
#define CPU_TSC     4  // RDTSC instruction support
#define CPU_MSR     5  // model specific registers
#define CPU_PAE     6  // physical address extension
#define CPU_MCE     7  // machine check exception
#define CPU_CX8     8  // compare and exchange 8B (CMPXCHG8)
#define CPU_APIC    9  // on chip APIC
#define CPU_SEP     11 // fast system call (SYSENTER, SYSEXIT)
#define CPU_MTRR    12 // memory type range registers
#define CPU_PGE     13 // page global enable
#define CPU_MCA     14 // machine check architecture
#define CPU_CMOV    15 // conditional move instruction
#define CPU_PAT     16 // page attribute table support
#define CPU_PSE36   17 // 36bit page size extension
#define CPU_PSN     18 // processor serial number
#define CPU_CLF     19 // CLFLUSH instruction
#define CPU_DTES    21 // debug store
#define CPU_ACPI    22 // ACPI (thermal monitor)
#define CPU_MMX     23 // MMX instruction set
#define CPU_FXSR    24 // FXSAVE, FXSTOR
#define CPU_SSE     25 // Streaming SIMD
#define CPU_SSE2    26 // Streaming SIMD 2
#define CPU_SS      27 // self snoop
#define CPU_HTT     28 // multithreading support
#define CPU_TM1     29 // thermal monitor
#define CPU_IA64    30 // actually reserved (!)
#define CPU_PBE     31 // pending break enable

#define CPU_SSE3    32 // streaming SIMD extensions 3
#define CPU_PCLMULDQ 33 // PCLMULDQ instruction
#define CPU_DTES64  34 // support for 64 bit branch history write
#define CPU_MONITOR 35 // MONITOR, MWAIT instructions
#define CPU_DS_CPL  36 // CPL qualified debug store
#define CPU_VMX     37 // virtualization (intel)
#define CPU_SMX     38 // trusted execution (intel)
#define CPU_EIST    39 // speed step (intel)
#define CPU_TM2     40 // thermal monitor 2
#define CPU_SSSE3   41 // supplemental streaming SIMD extensions 3
#define CPU_CNXT_ID 42 // L1 cache mode can be set
#define CPU_FMA     44 // fused multiply add
#define CPU_CX16    45 // CMPXCHG16B instruction
#define CPU_XTPR    46 // task priority messages update control
#define CPU_PDCM    47 // perfmon and debug capability
#define CPU_PCID    49 // process context identifiers
#define CPU_DCA     50 // direct cache access
#define CPU_SSE41   51 // streaming SIMD 4.1
#define CPU_SSE42   52 // streaming SIMD 4.2
#define CPU_X2APIC  53 // extended xAPIC support
#define CPU_MOVBE   54 // MOVBE instruction
#define CPU_POPCNT  55 // POPCNT instruction
#define CPU_TSC_DL  56 // ACI one shot using TSC deadline value
#define CPU_AES     57 // AES instruction extensions
#define CPU_XSAVE   58 // XSAVE, XSTOR proc extended feature, and others
#define CPU_OSXSAVE 59 // XSETBV, XGETBV enabled
#define CPU_AVX     60 // AVX instruction extensions
#define CPU_F16C    61 // 16bit float conversion
#define CPU_RDRAND  62 // RDRAND instruction

/* === OBTAIN CPU FLAGS === */
char *  cpu_vendor();
int     cpu_supports(int feature);

/* === DEBUGGING === */
void    print_cpu_flags();

#endif // SYSINFO_H
