#include "paging.h"
#include "memory/kheap.h"
#include "util/string.h"
#include "util/util.h"
#include "log.h"
#include "pagemgr.h"


int paging_enabled = 0;

typedef struct
{
    uint32_t entry_size;
    uint64_t base_addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;


static uint32_t available_memory;
static pagedir_t *pagedir_kernel;

extern void paging_enable(void);
void page_fault_handler(uint32_t error, uint32_t fault_addr);
pagedir_t *mk_kernel_pagedir(void);
static void pagetables_map(int n, int offset, pagedir_t *pagedir, int flags, uint32_t phys_addr);
static pagetable_entry_t *mk_pagetables(int n, int offset, pagedir_t *pagedir, int flags, char *description);

void setup_paging(uint32_t phys_memory)
{
    available_memory = phys_memory;
    setup_pagemgr(phys_memory); // MUST be called before paging_enable()

    pagedir_kernel = mk_kernel_pagedir();
    apply_pagedir(pagedir_kernel);

    paging_enable();
    paging_enabled = 1;
}

static pagetable_t *get_pagetable(int offset, pagedir_t *pagedir)
{
    return (pagetable_t*)(pagedir->pagetables[offset] & 0xfffff000);
}

static pagetable_entry_t *get_pagetable_entry(uint32_t addr, pagedir_t *pagedir)
{
    uint32_t pagedir_offset = addr >> 22;
    uint32_t pagetable_offset = (addr >> 12) & 0x000003ff;
    return &(get_pagetable(pagedir_offset, pagedir)->pages[pagetable_offset]);
}

pagedir_t *mk_kernel_pagedir()
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

void page_fault_handler(uint32_t error, uint32_t fault_addr)
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
            pagetable_entry_t *entry = get_pagetable_entry(fault_addr, pagedir_kernel);
            *entry = get_free_page(PAG_SUPV | PAG_RDWR);
            klog(KLOG_DEBUG, "allocated new page for kernel heap: virt(0x%x) -> phys(0x%x)",
                 fault_addr & 0xfffff000,
                 *entry & 0xfffff000);
        }
        else
        {
            // kernel tried to access an unallocated memory region
            // yet the behaviour is undefined
            klog(KLOG_PANIC, "kernel tried to access non-mapped memory region (%x)", fault_addr);
        }
    }
}



/*
 * pagetables_map(): maps 'n' pagetables starting with 'tables' to 'phys_addr'.
 */
static void pagetables_map(int n, int offset, pagedir_t *pagedir, int flags, uint32_t phys_addr)
{
    if (phys_addr % PAGESIZE != 0)
    {
        klog(KLOG_FAILURE, "pagetables_map(): cannot map to non-page-aligned physical address");
    }

    pagetable_t *table;
    uint32_t page_addr;
    for (int j, i = 0; i < n; i++)
    {
        table = get_pagetable(offset + i, pagedir);
        for (j = 0; j < 1024; j++)
        {
            page_addr = phys_addr + (MB4 * i) + (PAGESIZE * j);
            table->pages[j] = page_addr | PAG_PRESENT | flags;
        }
    }
}

/*
 * mk_pagetables(): creates 'n' pagetables at 'offset' in  page directory 'pagedir'
 * uses 'flags' and the heap 'description' accordingly.
 * returns: pointer to the first allocated page table
 */
static pagetable_entry_t *mk_pagetables(int n, int offset, pagedir_t *pagedir, int flags, char *description)
{
    void *pagetables = kmalloc(sizeof(pagetable_t) * n, PAGESIZE, description);
    memset(pagetables, 0, sizeof(pagetable_t) * n);
    for (int i = 0; i < n; i++)
    {
        pagetable_t *pgtable = (pagetable_t*)(pagetables + (sizeof(pagetable_t) * i));
        pagedir->pagetables[offset + i] = (uint32_t)pgtable | PAG_PRESENT | flags;
    }
    return pagetables;
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

    klog(KLOG_DEBUG, "setup_memory(): detected %S of physical memory", physical_size);
    return physical_size;
}
