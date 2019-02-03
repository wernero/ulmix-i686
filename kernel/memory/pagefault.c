#include <util/util.h>
#include <kdebug.h>
#include <sched/task.h>
#include <sched/process.h>
#include <sched/scheduler.h>
#include <memory/pagemgr.h>
#include <memory/paging.h>

extern pagedir_t *pagedir_kernel;
extern thread_t  *current_thread;

static int alloc_page(unsigned long fault_addr, pagedir_t *pagedir, int flags);

/*
 * page_fault_handler(): the page fault handler is called when the CPU issues
 * a page fault. the interrupt handler is configured to run as a gate, meaning
 * that no interrupts occur during the execution of a page fault handler.
 */
int page_fault_handler(unsigned long error, unsigned long fault_addr)
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

        klog(KLOG_INFO, "kill [%d]: uncorrectable page fault. addr=0x%x",
             current_thread->process->tgid,
             fault_addr);

        kill_process(current_thread->process);
        return -1;
    }
    else
    {
        if (fault_addr >= GB3)
        {
            // kernel tried to access heap that's not yet allococated.
            if (pheap_valid_addr(fault_addr))
            {
                return alloc_page(fault_addr, pagedir_kernel, PAG_SUPV | PAG_RDWR);
            }
            else
            {
                klog(KLOG_WARN, "kernel accessed non allocated heap location 0x%x (corrupt pointer?)",
                     fault_addr);
            }
        }
        else
        {
            if (current_thread == NULL || current_thread->process->pagedir == pagedir_kernel)
            {
                klog(KLOG_WARN, "kernel tried to access unmapped memory region (%x)", fault_addr);
            }
            else
            {
                return alloc_page(fault_addr, current_thread->process->pagedir, PAG_USER | PAG_RDWR);
            }
        }
    }
    return -1;
}

static int alloc_page(unsigned long fault_addr, pagedir_t *pagedir, int flags)
{
    struct paginfo_struct info;
    if (vaddr_info(pagedir, fault_addr, &info) == -PAG_ENOTAB)
    {
        // create pagetable
        mk_pagetables(1, info.pagedir_offset, pagedir, flags, "pagetable_t");
        if (vaddr_info(pagedir, fault_addr, &info) == -PAG_ENOTAB)
        {
            klog(KLOG_PANIC, "alloc_page(): cannot allocate page");
            return -1;
        }
    }

    // allocate page frame
    pagetable_entry_t entry;
    if (get_free_page(&entry, flags) < 0)
    {
        klog(KLOG_PANIC, "no more pages available");
        return -1;
    }

    // change page table
    info.pagetable->pages[info.pagetable_offset] = entry;
    /*klog(KLOG_DEBUG, "pagetables changed: virt(0x%x) -> phys(0x%x)",
         fault_addr & 0xfffff000,
         entry & 0xfffff000);*/
    return 0;
}
