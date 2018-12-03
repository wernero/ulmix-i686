#include "util/util.h"
#include "util/types.h"
#include "video/video.h"
#include "memory/gdt.h"
#include "memory/paging.h"
#include "interrupts.h"
#include "timer.h"
#include "cpu.h"
#include "log.h"
#include "sched/task.h"
#include "sched/scheduler.h"
#include "drivers/devices.h"

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


void test1(void)
{
    // Test function for scheduler
    klog(KLOG_DEBUG, "test function 1 running");
    for (;;) hlt();
}

void test2(void)
{
    // Test function for scheduler
    klog(KLOG_DEBUG, "test function 2 running");
    for (;;) hlt();
}

void main(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));
    klog(KLOG_INFO, "kernel loaded at 0x%x, size=%S",
         (int)&_kernel_beg,
         (int)&_kernel_end - (int)&_kernel_beg);

    console_t console;
    console_init(&console, YELLOW, CYAN);
    console_clear(&console);

    kprintf("ULMIX Operating System.\n");

    setup_gdt();
    setup_idt();
    setup_timer();
    // setup_cpu();

    uint32_t ram_available = setup_memory(mb_struct->mmap, mb_struct->mmap_length);
    setup_paging(ram_available);

    // scan for available devices and
    // automatically configure them
    scan_devices();
    kprintf("paging enabled\n");


    /* TESTING SCHEDULER */
    klog(KLOG_DEBUG, "creating process 1");
    int esp_test1 = 0;
    mk_process(get_kernel_pagedir(), test1, esp_test1, "test process 1");

    scheduler_enable();




    /* infinite loop - won't be executed because scheduler doesn't recognize
        this as a task and wont switch to it */
    for (;;) hlt();
}

