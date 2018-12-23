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
#include "kdebug.h"
#include "filesystem/vfscore.h"
#include "filesystem/fs_syscalls.h"
#include <errno.h>
#include <exec.h>

// .bss (ld)
extern char _bss_start;
extern char _bss_end;
extern char _kernel_beg;
extern char _kernel_end;

extern pagedir_t *pagedir_kernel;
static process_t *kprocess;

static void exec_init(void);

static void kmainthread(void)
{
    // Test function for scheduler
    klog(KLOG_INFO, "Welcome to the kernel main thread, my esp = 0x%x", get_esp());


    scan_devices();
    vfs_init();

    // Syscall Test
    int ret;
    __asm__("mov $0x02, %%eax;"
            "int $0x80;"
            "mov %%eax, %0" : "=r"(ret));
    klog(KLOG_DEBUG, "syscall test returned %d (5=success)", ret);

    exec_init();
    heap_dump();
    for (;;) hlt();
}

static void exec_init(void)
{

    char bin_to_exec[] = "/bin/init";

    // run init
    klog(KLOG_INFO, "executing /bin/init");
    pagedir_t *init_pd;
    if ((init_pd = mk_user_pagedir()) == NULL)
    {
        klog(KLOG_WARN, "failed to run /bin/init: could not create pagedir");
    }

    void *init_entry;
    //unsigned long init_esp = GB3;
    int err;
    if ((err = exec_load_img(init_pd, bin_to_exec, &init_entry)) < 0)               // "/bin/init" as const has not ending 0 as string end
    {
        kfree(init_pd);
        klog(KLOG_WARN, "failed to load /bin/init: errno %d", -err);
    }

    //mk_process(init_pd, TYPE_USER, init_entry, PAGESIZE, init_esp, "init");
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

    // creating kernel process
    kprocess = mk_process(pagedir_kernel,   // address space
                          TYPE_KERNEL,      // process type
                          kmainthread,      // entry point
                          2048,             // kernel stack size
                          -1,               // esp = kernel stack esp
                          "[KERNEL]");
    mk_thread(kprocess, mk_kstack(TYPE_KERNEL, idle_task, 1024, -1, get_eflags()), "idle task");

    // activate preemtive multitasking and force the
    // scheduler to preemt this function and never restore it
    scheduler_enable();
    scheduler_force();
    for (;;) hlt();
}

static void boot(multiboot_t* mb_struct)
{
    bzero(&_bss_start, (&_bss_end) - (&_bss_start));

    setup_gdt();

    kdebug_init();
    klog(KLOG_INFO, "ULMIX boot");
    klog(KLOG_INFO, "kernel loaded at 0x%x, size=%S",
         (int)&_kernel_beg,
         (int)&_kernel_end - (int)&_kernel_beg);

    setup_idt();
    setup_timer();
    // setup_cpu();

    uint32_t ram_available = setup_memory(mb_struct->mmap, mb_struct->mmap_length);
    setup_paging(ram_available);
}

