#include "util/util.h"
#include "memory/gdt.h"
#include "memory/kheap.h"
#include "memory/paging.h"
#include "interrupts.h"
#include "timer.h"
#include "cpu.h"
#include "log.h"
#include "sched/task.h"
#include "sched/scheduler.h"
#include "drivers/devices.h"

#include "video/video.h"

// .bss (ld)
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
extern char _kernel_end;

static void kmainthread(void)
{
    // Test function for scheduler
    klog(KLOG_DEBUG, "Welcome to the kernel main thread, my esp = 0x%x", get_esp());

    // scan for available devices and
    // automatically configure them
    scan_devices();

    for (;;) hlt();
}

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

static void boot(multiboot_t* mb_struct);

void main(multiboot_t* mb_struct)
{
    boot(mb_struct);

    // create the kernel main thread on this stack
    process_kstack_t kmain_stack =
    {
        .esp = MB1*5,
        .ebp = MB6,
        .kstack = NULL
    };
    kmain_stack = kstack_init(kmain_stack, TYPE_KERNEL, kmainthread, 0, get_eflags());
    mk_kernel_thread(kmain_stack, "kernel main thread");

    // activate preemtive multitasking and wait for the
    // scheduler to preemt this function and never restore it
    scheduler_enable();
    for (;;) hlt();
}

static void boot(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    klog(KLOG_INFO, "ULMIX boot");
    klog(KLOG_INFO, "kernel loaded at 0x%x, size=%S",
         (int)&_kernel_beg,
         (int)&_kernel_end - (int)&_kernel_beg);

    setup_gdt();
    //setup_debugger();
    setup_idt();
    setup_timer();
    // setup_cpu();

    uint32_t ram_available = setup_memory(mb_struct->mmap, mb_struct->mmap_length);
    setup_paging(ram_available);
}

