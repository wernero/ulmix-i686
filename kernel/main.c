#include "util/util.h"
#include "util/types.h"
#include "video/video.h"
#include "log.h"

// .bss (ld)
extern char _bss_start;
extern char _bss_end;

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


void main(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    console_t console;
    console_init(&console, YELLOW, CYAN);
    console_clear(&console);

    kprintf("***  ULMIX OPERATING SYSTEM v0.1  ***\n\n");

    klog(KLOG_INFO, "installing global descriptor table");
    // setup_gd();

    klog(KLOG_INFO, "setting up interrupt vector table");
    //setup_isr();

    klog(KLOG_INFO, "configuring CPU");
    //setup_cpu();*/

    klog(KLOG_INFO, "memory check");
    // memory_setup()

    klog(KLOG_INFO, "setting up paging");
    // paging


    //
    // TODO: debugging facilities
    //

    cli();
    hlt();
}

