#ifndef GDT_H
#define GDT_H

#include "util/types.h"

#define VALID   0x80

#define OFF_32  0x40
#define OFF_16  0x00

#define SYS_NON 0x10
#define SEG_SYS 0x00

#define ACC_SUP 0x00
#define ACC_USR 0x60

#define GRAN_4K 0x80
#define GRAN_BT 0x00

#define CXR     0x0A
#define DRW     0x02

typedef struct
{
    uint16_t size;
    uint32_t addr;
} __attribute__((packed)) gdt_descriptor_t;

typedef struct
{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct
{
    uint32_t prev_tss;   // for hardware multitasking
    uint32_t esp0;       // kernel SP
    uint32_t ss0;        // kernel SS
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;         // kernel ES
    uint32_t cs;         // kernel CS
    uint32_t ss;         // kernel SS
    uint32_t ds;         // kernel DS
    uint32_t fs;         // kernel FS
    uint32_t gs;         // kernel GS
    uint32_t ldt;        // unused: no LDT
    uint16_t trap;
    uint16_t iomap_base;
} __attribute__((packed)) tss_entry_t;

void setup_gdt(void);
void update_tss(uint32_t esp0);

void gdt_write(gdt_descriptor_t *gdt_ptr);
void tss_write(void);

#endif // GDT_H
