#include "task.h"
#include "scheduler.h"
#include "process.h"

#include <kdebug.h>

#define DESC_LEN 32


thread_t *current_thread = NULL;

/*
 * mk_thread(): create a thread_t structure with the specified values and insert
 * it into the scheduler queue (i.e. run it)
 * 'process'
 */
thread_t *mk_thread(process_t *process, kstack_t kstack, char *description)
{
    thread_t *thread = kmalloc(sizeof(thread_t), 1, "thread_t");
    strcpy(thread->description, description);
    thread->description[DESC_LENGTH - 1] = 0;
    thread->next_thread = NULL;
    thread->state = RUNNING;
    thread->tid = ++process->thread_count;
    thread->priority = 0; // not yet used
    thread->process = process;
    thread->kstack = kstack;
    scheduler_insert(thread);
    return thread;
}

/*
 * mk_kstack(): allocates a per-thread kernel stack and initializes it with specified values.
 * to initialize the stack, mk_kstack() internally calls kstack_init() to do the initialization.
 * See that function's reference for an overview.
 */
kstack_t mk_kstack(thread_type_t thread_type,
                           void *entry,
                           size_t stack_size,
                           unsigned long user_esp,
                           unsigned long eflags,
                           unsigned long eax)
{
    kstack_t kstack;
    kstack.kstack = (uint32_t)kmalloc(stack_size, 1, "thread_t kstack");
    kstack.ebp    = (uint32_t)(kstack.kstack + stack_size);
    kstack.esp    = kstack.ebp;
    return kstack_init(kstack, thread_type, entry, user_esp, eflags, eax);
}

/*
 * kstack_init(): initializes a per-thread kernel stack (kstack_t) with specified values.
 * the complete state of a preemted thread is stored on that stack. this function is used
 * to create a thread by putting initialization values on the per-thread kernel stack and
 * then calling 'iret' to resume the process's state.
 * argument 'kstack' has to have member 'esp' set to the init destination.
 * kstack_init() will chose the correct segments to be loaded depending on the 'thread_type',
 * which can either be TYPE_KERNEL or TYPE_USER.
 * 'start_addr' defines the entry point of the thread.
 * 'user_esp' initial stack pointer (only used when 'thread_type' is TYPE_USER).
 * 'eflags': initial EFLAGS state of the thread
 */
kstack_t kstack_init(kstack_t kstack, int thread_type, void *start_addr, unsigned long user_esp,
                     unsigned long eflags, unsigned long eax)
{
    int kernel_thread;
    uint32_t cs, ds;
    switch(thread_type)
    {
    case TYPE_KERNEL:
        kernel_thread = 1;
        cs = 0x08;
        ds = 0x10;
        break;
    case TYPE_USER:
    default:
        kernel_thread = 0;
        ds = 0x23;
        cs = 0x1b;
    }

    uint32_t *ksp = (uint32_t*)kstack.esp;
    kstack.ebp = (uint32_t)ksp;

    if (!kernel_thread)
    {
        // these values are only pushed onto the per-thread
        // kernel stack by user mode tasks
        *(--ksp) = ds;                  // user mode ss  = user ds
        *(--ksp) = user_esp;            // user mode esp = user_esp
    }

    *(--ksp) = eflags;                  // eflags
    *(--ksp) = cs;                      // cs
    *(--ksp) = (uint32_t)start_addr;    // eip

    // the general purpose registers are left uninitialized
    // but should be already set to zero by the heap manager
    *(--ksp) = eax;
    ksp -= 7;

    *(--ksp) = ds;    // ds = ds
    *(--ksp) = ds;    // es = ds
    *(--ksp) = ds;    // fs = ds
    *(--ksp) = ds;    // gs = ds

    kstack.esp = (uint32_t)ksp;
    return kstack;
}

// does not kill process
void kill_thread(thread_t *thread)
{
    scheduler_disable();

    thread->process->thread_count--;
    kfree(thread);

    scheduler_remove(thread);
    //scheduler_enable();
}
