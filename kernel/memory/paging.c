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
static pagedir_t *mk_kernel_pagedir(void);
static void pagetables_map(pagetable_t *tables, int n, int flags, uint32_t phys_addr);
static pagetable_t *mk_pagetable(pagedir_t *dir, int entry, uint32_t flags, char *description);
static pagetable_t *mk_pagetables(int n, int pgdir_offset, pagedir_t *dir, uint32_t flags, char *description);

extern void paging_enable(void);

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
            uint32_t pagedir_offset = (fault_addr >> 22) & 0x3ff;
            uint32_t pagetab_offset = (fault_addr >> 12) & 0x3ff;
            pagetable_t *pgtable = (pagetable_t*)(pagedir_kernel[pagedir_offset] & 0xfffff000);
            uint32_t *entry = &(pgtable[pagetab_offset]);
            *entry = get_free_page(PAG_SUPV | PAG_RDRW);
        }
        else
        {
            // kernel tried to access an unallocated memory region
            // yet the behaviour is undefined
            klog(KLOG_PANIC, "kernel tried to access non-mapped memory region (%x)", fault_addr);
        }
    }
}

void setup_paging(uint32_t phys_memory)
{
    available_memory = phys_memory;
    setup_pagemgr(phys_memory); // MUST be called before paging_enable()

    mk_kernel_pagedir();
    apply_pagedir(pagedir_kernel);

    paging_enable();
    paging_enabled = 1;
}

/*
 * mk_kernel_pagedir(): creates a page directory for use within the kernel
 * virtual memory layout:
 *  0   - 16M   identity mapping (statically allocated for the kernel)
 *  16M - 3G    unallocated
 *  3G  - 4G    kernel heap (initially not present)
 */
static pagedir_t *mk_kernel_pagedir(void)
{
    pagedir_kernel = kmalloc(sizeof(pagedir_entry_t) * 1024,
                                              4096,
                                              "PGDIR kernel");
    memset(pagedir_kernel, 0, sizeof(pagedir_entry_t) * 1024);


    // 4 page tables -> 16 MB of mapped memory
    pagetable_t *stables;
    stables = mk_pagetables(4, 0, pagedir_kernel, PAG_PRESENT | PAG_SUPV | PAG_RDRW, "pgtables kstatic");
    pagetables_map(stables, 4, PAG_SUPV | PAG_RDRW, 0);

    // 256 page tables -> 1 GB of mapped memory at vaddr 3GB-4GB
    mk_pagetables(256, 768, pagedir_kernel, PAG_PRESENT | PAG_SUPV | PAG_RDRW, "pgtables kheap");

    return pagedir_kernel;
}

/*
 * pagetables_map(): maps 'n' pagetables starting with 'tables' to 'phys_addr'.
 */
static void pagetables_map(pagetable_t *tables, int n, int flags, uint32_t phys_addr)
{
    if (phys_addr % PAGESIZE != 0)
    {
        klog(KLOG_FAILURE, "pagetables_map(): cannot map to non-page-aligned physical address");
    }

    pagetable_t *table;
    for (int i = 0; i < n; i++)
    {
        table = (pagetable_t*)((char*)tables + PGTABLE_SIZE * i);
        for (int j = 0; j < 1024; j++)
        {
            table[j] = (phys_addr + ((i*4096*1024) + (j*4096))) | PAG_PRESENT | flags;
        }
    }
}

/*
 * mk_user_pagedir(): creates a page directory for use in user-space
 * virtual memory layout:
 * 0 - 16 MB    identity mapping (supervisor access only)
 * 16MB - ...   executable image + init_heap
 * ... - 3GB    init_stack + environment
 * 3GB - 4GB    kernel heap (supervisor access only)
 */
pagedir_t *mk_user_pagedir(uint32_t exec_size, uint32_t init_stack_size, uint32_t init_heap_size)
{
    pagedir_t *pagedir_user = kmalloc(sizeof(pagedir_entry_t) * 1024,
                                              4096,
                                              "PGDIR user");
    memset(pagedir_kernel, 0, sizeof(pagedir_entry_t) * 1024);

    int i, j;
    // 0 - 16MB Kernel Space
    for (i = 0; i < 4; i++)
    {
        pagedir_user[i] = pagedir_kernel[i];
    }

    // 3GB - 4GB Kernel Space
    for (i = 256*3; i < 1024; i++)
    {
        pagedir_user[i] = pagedir_kernel[i];
    }

    int pages_remaining;
    int pages = pages_remaining = (exec_size + init_stack_size) / PAGESIZE;
    for (i = 4; i < 4 + (pages / 1024) + 1; i++)
    {
        pagetable_t *pt = mk_pagetable(pagedir_user, i, PAG_PRESENT | PAG_RDRW, "PGT user");
        for (j = 0; j < pages_remaining && j < 1024; j++)
        {
            pt[j] = get_free_page(PAG_USER | PAG_RDRW);
        }
        pages_remaining -= 1024;
    }

    pages = pages_remaining = init_heap_size / PAGESIZE;
    for (i = ((256 * 3) - ((pages / 1024) + 1)); i < 256*3; i++)
    {
        pagetable_t *pt = mk_pagetable(pagedir_user, i, PAG_PRESENT | PAG_RDRW, "PGT userheap");
        for (j = 0; j < pages_remaining && j < 1024; j++)
        {
            pt[j] = get_free_page(PAG_USER | PAG_RDRW);
        }
        pages_remaining -= 1024;
    }

    return pagedir_user;
}



/*
 * OBSOLETE (!) allocates page tables not efficiently
 * mk_pagetable(): creates a page table on the kernel heap
 *   and assigns it to a specified page directory.
 * 'flags' will be set accordingly.
 * 'description' will show up on debug output of the kernel heap.
 * 'entry' is the entry-offset of the destination page directory.
 * 'dir' is the destination page directory
 */
static pagetable_t *mk_pagetable(pagedir_t *dir, int entry, uint32_t flags, char *description)
{
    pagetable_t *pagetable = kmalloc(sizeof(pagetable_entry_t) * 1024, PAGESIZE, description);
    memset(pagetable, 0, sizeof(pagetable_entry_t) * 1024);
    dir[entry] = (uint32_t)(((uint32_t)pagetable) | flags);
    return pagetable;
}

/*
 * mk_pagetables(): creates 'n' pagetables at offset 'pgdir_offset' in  page directory 'dir'
 * uses 'flags' and the heap 'description' accordingly.
 * returns: pointer to the first allocated page table
 */
static pagetable_t *mk_pagetables(int n, int pgdir_offset, pagedir_t *dir, uint32_t flags, char *description)
{
    pagetable_t *pgtable;
    pagetable_t *pgtables = kmalloc(PGTABLE_SIZE * n, PAGESIZE, description);
    memset(pgtables, 0, PGTABLE_SIZE * n);
    for (int i = pgdir_offset; i < pgdir_offset + n; i++)
    {
        pgtable = (pagetable_t *)((char*)pgtables + PGTABLE_SIZE * i);
        dir[i] = (uint32_t)((uint32_t)pgtable | flags);
    }
    return pgtables;
}

uint32_t setup_memory(void *mmap, uint32_t mmap_len)
{
    klog(KLOG_DEBUG, "scanning physical memory");
    uint32_t physical_size = 0;
    mmap_entry_t *end = (mmap_entry_t*)((char*)mmap + mmap_len);
    for (mmap_entry_t *entry = mmap; entry < end; entry = (mmap_entry_t*)((char*)entry + entry->entry_size + 4))
    {
        klog(KLOG_DEBUG, "-> mmap: addr=%X, free=%d, size=%S", entry->base_addr, entry->type, entry->size);
        if (entry->base_addr <= GB4 && entry->size != 0)
        {
            if (entry->base_addr + entry->size >= GB4)
            {
                entry->size = GB4 - entry->base_addr;
            }
        }
        physical_size += entry->size;
    }

    klog(KLOG_DEBUG, "setup_memory(): physical memory size of %S", physical_size);
    return physical_size;
}

void apply_pagedir(void *pagedir)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}
