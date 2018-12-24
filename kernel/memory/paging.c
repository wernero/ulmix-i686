#include "paging.h"
#include <util/util.h>
#include <memory/kheap.h>
#include <memory/pagemgr.h>
#include <log.h>

typedef struct
{
    uint32_t entry_size;
    uint64_t base_addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

int paging_enabled = 0;
pagedir_t *pagedir_kernel;
static unsigned long available_memory;

extern void paging_enable(void);

static pagedir_t *mk_kernel_pagedir(void);
static void pagetables_map(int count, int pagedir_offset, pagedir_t *pagedir, int flags, unsigned long phys_addr);
static pagetable_entry_t *mk_pagetables(int count, int pagedir_offset, pagedir_t *pagedir, int flags, char *description);
static pagetable_t *get_pagetable(int pagedir_offset, pagedir_t *pagedir);
static pagetable_entry_t *get_pagetable_entry(uint32_t addr, pagedir_t *pagedir);

void page_fault_handler(uint32_t error, unsigned long fault_addr)
{
    if (error & 4) // occured in user mode?
    {
        if (error & 1)
        {
            // user tried to access kernel space
            // send error and kill task
        }
        else
        {
            // page not present

            if (1)
            {
                // check whether the user has a right to
                // access and fix it
            }
            else
            {
                // issue SIGSEGV and kill
            }
        }
    }
    else
    {
        if (fault_addr >= GB3)
        {
            // kernel tried to access heap that's not yet allococated.
            if (pheap_valid_addr(fault_addr))
            {
                pagetable_entry_t *entry = get_pagetable_entry(fault_addr, pagedir_kernel);
                *entry = get_free_page(PAG_SUPV | PAG_RDWR);
                klog(KLOG_DEBUG, "allocated new page for kernel heap: virt(0x%x) -> phys(0x%x)",
                     fault_addr & 0xfffff000,
                     *entry & 0xfffff000);
            }
            else
            {
                klog(KLOG_PANIC, "kernel accessed non allocated heap location 0x%x (corrupt pointer?)",
                     fault_addr);
            }
        }
        else
        {
            // kernel tried to access an unallocated memory region
            // yet the behaviour is undefined
            klog(KLOG_PANIC, "kernel tried to access unmapped memory region (%x)", fault_addr);
        }
    }
}

void setup_paging(uint32_t phys_memory)
{
    available_memory = phys_memory;
    setup_pagemgr(phys_memory); // MUST be called before paging_enable()

    pagedir_kernel = mk_kernel_pagedir();
    apply_pagedir(pagedir_kernel);

    paging_enable();
    paging_enabled = 1;
    klog(KLOG_INFO, "Paging enabled");
}

pagedir_t *pagedir_copy(pagedir_t *source)
{
    pagedir_t *udir = kmalloc(sizeof(pagedir_t), PAGESIZE, "user pagedir");

    // map kernel regions into user pagedir (0-16M, 3-4G)
    memcpy(udir, pagedir_kernel, sizeof(pagedir_entry_t) * 4);
    memcpy((pagedir_t *)((char*)udir + sizeof(pagedir_entry_t) * 768),
           pagedir_kernel,
           sizeof(pagedir_entry_t) * 256);

    // go through every page table entry, and if it's present, copy it
    /*for (int j, i = 4; i < 768; i++)
    {
        for (int j = 0; j < 1024; j++)
        {
            pagedir_entry_t pde = source->pagetables[i];
            if (pde & 1) // pagetable is present?
            {
                pagetable_t *ptp = (pagetable_t *)(pde & 0xfffff000);
                pagetable_entry_t pte = ptp->pages[j];
                if (pte & 1) // pagetable entry is present?
                {
                    pagetable_entry_t pte_new = get_free_page(PAG_USER | PAG_RDWR);
                    pte_new
                }
            }
        }
    }*/
    return udir;
}

pagedir_t *get_kernel_pagedir()
{
    return pagedir_kernel;
}

uint32_t setup_memory(void *mmap, uint32_t mmap_len)
{
    uint32_t physical_size = 0;
    mmap_entry_t *end = (mmap_entry_t*)((char*)mmap + mmap_len);
    for (mmap_entry_t *entry = mmap; entry < end; entry = (mmap_entry_t*)((char*)entry + entry->entry_size + 4))
    {
        if (entry->base_addr <= GB4 && entry->size != 0)
        {
            if (entry->base_addr + entry->size >= GB4)
            {
                entry->size = GB4 - entry->base_addr;
            }
        }
        physical_size += entry->size;
    }

    klog(KLOG_INFO, "setup_memory(): detected %S of physical memory", physical_size);
    return physical_size;
}

/*
 * mk_pagetables(): creates 'n' pagetables at 'offset' in  page directory 'pagedir'
 * uses 'flags' and the heap 'description' accordingly.
 * returns: pointer to the first allocated page table
 */
static pagetable_entry_t *mk_pagetables(int count, int pagedir_offset, pagedir_t *pagedir, int flags, char *description)
{
    void *pagetables = kmalloc(sizeof(pagetable_t) * count, PAGESIZE, description);
    memset(pagetables, 0, sizeof(pagetable_t) * count);
    for (int i = 0; i < count; i++)
    {
        pagetable_t *pgtable = (pagetable_t*)(pagetables + (sizeof(pagetable_t) * i));
        pagedir->pagetables[pagedir_offset + i] = (uint32_t)pgtable | PAG_PRESENT | flags;
    }
    return pagetables;
}

/*
 * pagetables_map(): maps 'n' pagetables starting with 'tables' to 'phys_addr'.
 */
static void pagetables_map(int n, int pagedir_offset, pagedir_t *pagedir, int flags, unsigned long phys_addr)
{
    if (phys_addr % PAGESIZE != 0)
    {
        klog(KLOG_FAILURE, "pagetables_map(): cannot map to non-page-aligned physical address");
    }

    pagetable_t *table;
    uint32_t page_addr;
    for (int j, i = 0; i < n; i++)
    {
        table = get_pagetable(pagedir_offset + i, pagedir);
        for (j = 0; j < 1024; j++)
        {
            page_addr = phys_addr + (MB4 * i) + (PAGESIZE * j);
            table->pages[j] = page_addr | PAG_PRESENT | flags;
        }
    }
}

static pagetable_t *get_pagetable(int pagedir_offset, pagedir_t *pagedir)
{
    return (pagetable_t*)(pagedir->pagetables[pagedir_offset] & 0xfffff000);
}

static pagetable_entry_t *get_pagetable_entry(uint32_t addr, pagedir_t *pagedir)
{
    uint32_t pagedir_offset = addr >> 22;
    uint32_t pagetable_offset = (addr >> 12) & 0x000003ff;
    return &(get_pagetable(pagedir_offset, pagedir)->pages[pagetable_offset]);
}

static pagedir_t *mk_kernel_pagedir()
{
    pagedir_t *kdir = kmalloc(sizeof(pagedir_t), PAGESIZE, "kernel pagedir");

    mk_pagetables(4, 0, kdir, PAG_SUPV | PAG_RDWR, "pgtables kstatic");
    pagetables_map(4, 0, kdir, PAG_SUPV | PAG_RDWR, 0x00);

    mk_pagetables(256, 768, kdir, PAG_SUPV | PAG_RDWR, "pgtables kheap");

    return kdir;
}

void apply_pagedir(void *pagedir)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}
