#include "gdt.h"
#include <ulmix.h>

extern void gdt_write(struct gdtd_struct *gdt_ptr);
extern void tss_write(void);

static struct gdte_struct gdt[GDT_ENTRIES];
static struct tsse_struct tss = {};

static void __init setup_entry(int id,
        uint32_t base, uint32_t limit,
        uint8_t access, uint8_t flags)
{
    gdt[id].base_low    = base & 0xffff;
    gdt[id].base_middle = (base >> 16) & 0xff;
    gdt[id].base_high   = (base >> 24) & 0xff;

    gdt[id].limit       = limit & 0xffff;
    gdt[id].flags       = (limit >> 16) & 0x0f;

    gdt[id].flags      |= (flags & 0xf0);
    gdt[id].access      = access;
}

void __init setup_gdt(void)
{
    struct gdtd_struct gdt_descr;
    gdt_descr.size = sizeof(struct gdte_struct) * GDT_ENTRIES - 1;
    gdt_descr.addr = (uint32_t)gdt;

    setup_entry(0, 0, 0, 0, 0); // null descriptor required by design

    setup_entry(1, 0, 0xFFFFF, VALID | ACC_SUP | SYS_NON | CXR, GRAN_4K | OFF_32);  // kcode
    setup_entry(2, 0, 0xFFFFF, VALID | ACC_SUP | SYS_NON | DRW, GRAN_4K | OFF_32);  // kdata
    setup_entry(3, 0, 0xFFFFF, VALID | ACC_USR | SYS_NON | CXR, GRAN_4K | OFF_32);  // ucode
    setup_entry(4, 0, 0xFFFFF, VALID | ACC_USR | SYS_NON | DRW, GRAN_4K | OFF_32);  // udata

    // task state segment
    setup_entry(5, (uint32_t)&tss, sizeof(tss), 0xe9, 0x00);

    tss.ss0  = 0x10;
    tss.esp0 = 0x00;
    tss.cs   = 0x08;
    tss.ss   =
    tss.ds   =
    tss.es   =
    tss.fs   =
    tss.gs   = 0x10;

    gdt_write(&gdt_descr);
    tss_write();
}

void update_tss(unsigned long sp)
{
    tss.esp0 = sp;
}
