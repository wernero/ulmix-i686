#include "paging.h"
#include <util/util.h>
#include <memory/kheap.h>
#include <memory/pagemgr.h>
#include <sched/task.h>
#include <sched/process.h>
#include <log.h>
#include <errno.h>

typedef struct
{
    uint32_t entry_size;
    uint64_t base_addr;
    uint64_t size;
    uint32_t type;
} __attribute__((packed)) mmap_entry_t;

int paging_enabled = 0;
pagedir_t *pagedir_kernel;
extern thread_t *current_thread;
static unsigned long available_memory;

extern void paging_enable(void);

static pagedir_t *mk_kernel_pagedir(void);
static void pagetables_map(int count, int pagedir_offset, pagedir_t *pagedir, int flags, unsigned long phys_addr);
static pagetable_t *get_pagetable(int pagedir_offset, pagedir_t *pagedir);




int vaddr_info(pagedir_t *pagedir, unsigned long addr, struct paginfo_struct *info)
{
    info->pagedir = pagedir;
    info->pagedir_offset     = addr >> 22;
    info->pagetable_offset   = (addr >> 12) & 0x000003ff;
    int status = SUCCESS;

    if (pagedir->pagetables[info->pagedir_offset] & 1)
    {
        info->pagetable = get_pagetable(info->pagedir_offset, pagedir);
        if (info->pagetable->pages[info->pagetable_offset] & 1)
            return SUCCESS;
        return -PAG_ENOTPR;
    }
    else
    {
        // no page table present
        status = -PAG_ENOTAB;
    }

    return status;
}
















void delete_pagedir(pagedir_t *pagedir)
{
    // TODO: delete page directory
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

pagedir_t *mk_user_pagedir(void)
{
    pagedir_t *udir = kmalloc(sizeof(pagedir_t), PAGESIZE, "user pagedir");

    // map kernel regions into user pagedir
    memcpy(udir, pagedir_kernel, sizeof(pagedir_t));

    return udir;
}

pagedir_t *pagedir_copy_current(void)
{
    klog(KLOG_DEBUG, "copying current page directory");
    pagedir_t *udir = mk_user_pagedir();
    pagedir_t *source = current_thread->process->pagedir;

    // search for unused page table
    int cp_index = -1;
    for (int i = 4; i < 768; i++)
    {
        if (source->pagetables[i] & 0x01)
            continue;

        cp_index = i;
        break;
    }

    if (cp_index == -1)
    {
        klog(KLOG_FAILURE, "pagedir_copy_current(): no available pagetable for copying");
        return NULL;
    }

    // iterate page tables
    pagetable_t *cp_table;
    pagetable_t *src_table;
    for (int i = 4; i < 768; i++)
    {
        // if page table is present, map it into
        // the current page directory
        if (source->pagetables[i] & 0x01 && i != cp_index)
        {
            // create & map into current pagedir
            cp_table = mk_pagetables(1, i, udir, PAG_USER | PAG_RDWR, "user pagetable_t");
            source->pagetables[cp_index] = get_physical(cp_table) | PAG_PRESENT | PAG_SUPV | PAG_RDWR;
            src_table = source->pagetable_ptrs[i];

            // iterate pages
            for (int j = 0; j < 1024; j++)
            {
                if (src_table->pages[j] & 0x01)
                {
                    klog(KLOG_DEBUG, "pagetable %d, page %d will be copied", i, j);

                    // allocate page and copy content
                    pagetable_entry_t e;
                    get_free_page(&e, PAG_RDWR | PAG_USER);
                    cp_table->pages[j] = e;

                    memcpy((void*)(cp_index*MB4 + j*PAGESIZE),  // destination addr (other pagedir)
                           (void*)(MB4*i + PAGESIZE*j),         // source addr (current pagedir)
                           PAGESIZE);
                }
                else
                {
                    cp_table->pages[j] = 0;
                }
            }
        }
        else
        {
            udir->pagetables[i] = 0;
        }
    }

    source->pagetables[cp_index] = 0;
    return udir;
}

pagedir_t *get_kernel_pagedir()
{
    return pagedir_kernel;
}

/*
 * mk_pagetables(): creates 'n' pagetables at 'offset' in  page directory 'pagedir'
 * uses 'flags' and the heap 'description' accordingly.
 * returns: pointer to the first allocated page table
 */
pagetable_t *mk_pagetables(int count, int pagedir_offset, pagedir_t *pagedir, int flags, char *description)
{
    void *pagetables = kmalloc(sizeof(pagetable_t) * count, PAGESIZE, description);
    memset(pagetables, 0, sizeof(pagetable_t) * count);
    for (int i = 0; i < count; i++)
    {
        pagetable_t *pgtable = (pagetable_t*)(pagetables + (sizeof(pagetable_t) * i));
        unsigned long phys_addr = get_physical(pgtable);
        pagedir->pagetables[pagedir_offset + i] = phys_addr | PAG_PRESENT | flags;
        pagedir->pagetable_ptrs[pagedir_offset + i] = pgtable;
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
    if (pagedir->pagetables[pagedir_offset] & 1)
        return pagedir->pagetable_ptrs[pagedir_offset];
    return NULL;
}

static pagedir_t *mk_kernel_pagedir()
{
    pagedir_t *kdir = kmalloc(sizeof(pagedir_t), PAGESIZE, "kernel pagedir");

    mk_pagetables(4, 0, kdir, PAG_SUPV | PAG_RDWR, "pgtables kstatic");
    pagetables_map(4, 0, kdir, PAG_SUPV | PAG_RDWR, 0x00);

    mk_pagetables(1, 768, kdir, PAG_SUPV | PAG_RDWR, "pgtable kheap");

    return kdir;
}














unsigned long get_physical(void * addr)
{
    if (current_thread == NULL)
        return (unsigned long)addr;

    struct paginfo_struct info;
    if (vaddr_info(current_thread->process->pagedir, (unsigned long)addr, &info) < 0)
        return NULL;

    uint32_t pt_entry = info.pagetable->pages[info.pagetable_offset];
    return (unsigned long)pt_entry & 0xfffff000;
}

void apply_pagedir(void *pagedir)
{
    if ((unsigned long)pagedir >= MB16)
    {
        pagedir = (void*)get_physical(pagedir);
    }

    __asm__ volatile ("mov %0, %%cr3" : : "r"(pagedir));
}


