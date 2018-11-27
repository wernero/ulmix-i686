#include "util/util.h"
#include "util/types.h"
#include "video/video.h"
#include "memory/gdt.h"
#include "log.h"
#include "interrupts.h"
#include "cpu.h"
#include "timer.h"

// .bss (ld)
extern char _bss_start;
extern char _bss_end;
extern char _kernel_end;

typedef struct
{
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    void*    mods;
    uint32_t syms[4];
    uint32_t mmap_length;
    void*    mmap;
    uint32_t drives_length;
    void*    drives;
    uint32_t config_table;
    char*    bootloader_name;
    uint32_t apmTable;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} __attribute__((packed)) multiboot_t;


void keyboard_handler(void)
{
    kprintf("TASTE GEDRUECKT\n");
}

void main(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    console_t console;
    console_init(&console, YELLOW, CYAN);
    console_clear(&console);

    kprintf("***  ULMIX OPERATING SYSTEM v0.1  ***\n\n");

    kprintf("Kernel loaded at %x, size=%dB\n", 0x100000, (int)&_kernel_end - 0x100000);
    kprintf("initializing system:\n");

    klog(KLOG_INFO, "GDT");
    setup_gdt();

    klog(KLOG_INFO, "IDT, ISR handlers");
    setup_idt();

    klog(KLOG_INFO, "System Timer\n");
    setup_timer();

    //klog(KLOG_INFO, "configuring CPU");
    //setup_cpu();
    //sti();//

/*    klog(KLOG_INFO, "memory check");
    // memory_setup()

    klog(KLOG_INFO, "setting up paging");
    // paging*/

//    pic_init();

    //
    // TODO: debugging facilities
    //

    //cli();
    for (;;)
        hlt();
}

