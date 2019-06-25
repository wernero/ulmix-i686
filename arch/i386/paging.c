#include <mem.h>
#include <debug.h>
#include <string.h>
#include <sysinfo.h>

#include "memory.h"
#include "paging.h"

#define PTR_TABLE_SIZE  32      // 4 entries x 8 Bytes

#define PAG_PAE     BIT(5)
#define PAG_WP      BIT(16)
#define PAG_NX      BIT(11)
#define PAG_SMEP    BIT(20)
#define PAG_ENABLE  BIT(31)

static int pae_support = 0;     // PAE (physical address extension)
static int smep_support = 0;    // SMEP (supervisor mode execution prevention)
static int nx_support = 0;      // NX (no execute bit)

struct mm_struct *current_mm; // mmap currently in use
static struct mm_struct *mm_kernel;  // mmap with kernel mappings

extern char _bss_end;

static void paging_enable()
{
    unsigned long cr0, cr4;
     __asm__ volatile ("mov %%cr0, %0;"
                       "mov %%cr4, %1;"
                       : "=r"(cr0), "=r"(cr4));

    // enable paging with the following settings:
    // CR0.WP = 1           // don't allow kernel to write readonly pages
    // CR4.SMEP = 1,        // protect kernel from executing user mode code
    // IA32_EFER.NXE = 1    // enable NX bit (no execute)
    // CR4.PAE = 1          // enable PAE (physical address extension)
    // CR0.PG = 1           // enable paging unit

    if (pae_support)    cr4 |= PAG_PAE;
    if (smep_support)   cr4 |= PAG_SMEP;

    if (nx_support);

    cr0 |= PAG_WP;
    cr0 |= PAG_ENABLE;
    __asm__ volatile ("mov %0, %%cr4;"
                      "mov %1, %%cr0;"
                      : : "r"(cr4), "r"(cr0));
}

static void apply_mmap(struct mm_struct *mmap)
{
    void *pagedir = mm_physical(mmap->tables->ptr_table);
    current_mm = mmap;
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}

void setup_paging(void)
{
    // check for PAE and MSR support
    pae_support = cpu_has(CPU_PAE);

    // for now, assume these are not supported:
    smep_support = 0;
    nx_support = 0;

    if (!pae_support)
    {
        // for now, panic
        panic("CPU does not support PAE");
    }

    setup_pfalloc();

    mm_kernel = mk_mmap("mm_kernel");

    /* actually map the kernel binary and data manually before
     * the activation of paging. otherwise, the CPU can't access
     * the page fault handler and the kernel will blow up -.- */
    mm_map(mm_kernel, 0x0, 0x0, (size_t)&_bss_end, PG_SUPV | PG_RDWR);

    apply_mmap(mm_kernel);
    paging_enable();
}


struct mm_struct *mk_mmap(const char *description)
{
    struct mm_struct *mm = kmalloc(sizeof(struct mm_struct), 1, description);
    mm->tables = kmalloc(sizeof(struct mm_tables_struct), 1, "mm_tables");

    mm->tables->page_dirs[0] = kmalloc(PAGEDIR_SIZE, PAGESIZE, "mm_pagedir0");
    bzero(mm->tables->page_dirs[0], PAGEDIR_SIZE);

    // page directory pointer table: create one entry that points to pagedir[0]
    // PDPT has to be aligned only on a 32 byte boundary
    mm->tables->ptr_table = kmalloc(PTR_TABLE_SIZE, 0x20, "mm_ptr_table");
    bzero(mm->tables->ptr_table, PTR_TABLE_SIZE);
    mm->tables->ptr_table[0] = (unsigned long)(mm->tables->page_dirs[0]) | PG_PRESENT;

    mm->regions = NULL;
    return mm;
}


