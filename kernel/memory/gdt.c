#include "gdt.h"
#include "log.h"


#define GDT_ENTRIES 6 // null, kcode, kdata, ucode, udata, tss

static gdt_entry_t gdt[GDT_ENTRIES];
static tss_entry_t tss = {};

static void setup_entry(int id,
                        uint32_t base,
                        uint32_t limit,
                        uint8_t access,
                        uint8_t flags)
{
    gdt[id].base_low    = base & 0xffff;
    gdt[id].base_middle = (base >> 16) & 0xff;
    gdt[id].base_high   = (base >> 24) & 0xff;

    gdt[id].limit       = limit & 0xffff;
    gdt[id].flags       = (limit >> 16) & 0x0f;

    gdt[id].flags      |= (flags & 0xf0);
    gdt[id].access      = access;
}

void setup_gdt()
{
    gdt_descriptor_t gdt_descr;
    gdt_descr.size = sizeof(gdt_entry_t) * GDT_ENTRIES - 1;
    gdt_descr.addr = (uint32_t)gdt;

    klog(KLOG_DEBUG, "Global Descriptor Table at address 0x%x", (uint32_t)&gdt_descr);

    setup_entry(0, 0, 0, 0, 0);

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
    tss_write(&tss);
}
