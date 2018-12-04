#include "task.h"
#include "sched/scheduler.h"

#include "log.h"

#define DESC_LEN 32

thread_t *current_thread = NULL;
static pid_t pid_counter = 1;

static pid_t new_pid(void);

static thread_t *mk_thread(thread_type_t thread_type, process_t *process, pid_t tid, process_kstack_t kstack, char *description);


process_t *mk_process(pagedir_t *pagedir, void (*entry)(void), uint32_t esp, char *description)
{
    uint32_t user_eflags = get_eflags(); // *** temporary!

    process_t *process = kmalloc(sizeof(process_t), 1, "process_t");
    strcpy(process->description, description);
    process->description[DESC_LENGTH - 1] = 0;
    process->pid = new_pid();
    process->threads = mk_thread(TYPE_USER, process, 0, mk_kstack(TYPE_USER, (void*)entry, 2048, esp, user_eflags), description);
    process->pagedir = pagedir;

    if (current_thread != NULL && current_thread->process != NULL)
        process->ppid = current_thread->process->pid;
    else
        process->ppid = 0;
    return process;
}

void kill_thread(thread_t *thread)
{
    scheduler_disable();
    if (thread == current_thread)
    {
        current_thread->state = KILLED;
        scheduler_force();
    }
    else
    {
        scheduler_remove(thread);
    }
    scheduler_enable();
}

/*
 * mk_kernel_thread(): creates a thread of type 'kernel' and starts it.
 * 'kstack' specifies it's stack.
 */
thread_t *mk_kernel_thread(process_kstack_t kstack, char *description)
{
    thread_t *kthread = mk_thread(TYPE_KERNEL, NULL, 0, kstack, description);
    return kthread;
}

/*
 * mk_thread(): create a thread_t structure with the specified values and insert
 * it into the scheduler queue (i.e. run it)
 * 'process'
 */
static thread_t *mk_thread(thread_type_t thread_type, process_t *process, pid_t tid, process_kstack_t kstack, char *description)
{
    thread_t *thread = kmalloc(sizeof(thread_t), 1, "thread_t");
    strcpy(thread->description, description);
    thread->description[DESC_LENGTH - 1] = 0;
    thread->next_thread = NULL;
    thread->state = RUNNING;
    thread->tid = tid;
    thread->priority = 0; // not yet used
    thread->type = thread_type;
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
process_kstack_t mk_kstack(thread_type_t thread_type, void *entry, size_t stack_size, uint32_t user_esp, uint32_t eflags)
{
    process_kstack_t kstack;
    kstack.kstack = (uint32_t)kmalloc(stack_size, 1, "thread_t kstack");
    kstack.ebp    = (uint32_t)(kstack.kstack + stack_size);
    kstack.esp    = kstack.ebp;
    return kstack_init(kstack, thread_type, entry, user_esp, eflags);
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
process_kstack_t kstack_init(process_kstack_t kstack, int thread_type, void *start_addr, uint32_t user_esp, uint32_t eflags)
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
        // these values are only pushed onto the pre-thread
        // kernel stack by user mode tasks
        *(--ksp) = ds;                  // user mode ss  = user ds
        *(--ksp) = user_esp;            // user mode esp = user_esp
    }

    *(--ksp) = eflags;                  // eflags
    *(--ksp) = cs;                      // cs
    *(--ksp) = (uint32_t)start_addr;    // eip

    // the general purpose registers are left uninitialized
    // but should be already set to zero by the heap manager
    ksp -= 8;

    *(--ksp) = ds;    // ds = ds
    *(--ksp) = ds;    // es = ds
    *(--ksp) = ds;    // fs = ds
    *(--ksp) = ds;    // gs = ds

    kstack.esp = (uint32_t)ksp;
    return kstack;
}

static pid_t new_pid()
{
    return pid_counter++;
}
