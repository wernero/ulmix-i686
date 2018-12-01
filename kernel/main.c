#include "util/util.h"
#include "util/types.h"
#include "video/video.h"
#include "memory/gdt.h"
#include "memory/paging.h"
#include "interrupts.h"
#include "timer.h"
#include "cpu.h"
#include "log.h"

#include "util/string.h"
#include "memory/kheap.h"

#include "drivers/serial.h"
#include "drivers/keyboard.h"

// .bss (ld)
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
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


void genfibonacci(void)
{

}

void main(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    klog(KLOG_DEBUG, "kernel loaded at 0x%x, size=%S\n", (int)&_kernel_beg, (int)&_kernel_end - (int)&_kernel_beg);
    klog(KLOG_DEBUG, "initializing console");
    console_t console;
    console_init(&console, YELLOW, CYAN);
    console_clear(&console);

    kprintf("***  ULMIX OPERATING SYSTEM v0.1  ***\n\n");
    kprintf("initializing system:\n");

    setup_gdt();
    setup_idt();
    setup_timer();

    /*klog(KLOG_INFO, "CPU features\n");
    setup_cpu();*/

    uint32_t ram_available = setup_memory(mb_struct->mmap, mb_struct->mmap_length);
    setup_paging(ram_available);

    kprintf("RAM: %dM usable\n", ram_available / (1024*1024));
    kprintf("paging enabled\n");

    //*((char *)0x1000000) = 5;
    char *test = kmalloc(5000, 1, "test1");
    strcpy(test, "Hello");
    kprintf("found %s\n", test);
    char *test2 = kmalloc(50, 1, "test2");
    strcpy(test2, "hei 2");
    kprintf("hihi %s\n", test2);


    //
    // TODO: debugging facilities
    //

    for (;;) hlt();
}

