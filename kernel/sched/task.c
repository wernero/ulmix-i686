#include "task.h"

#define DESC_LEN 32

thread_t *current_thread;
static pid_t pid_counter = 1;

static pid_t new_pid(void);
static thread_t *mk_thread(process_t *process, pid_t tid, void (*entry)(void), uint32_t esp, char *description);
static process_kstack_t mk_kstack(uint32_t user_esp, uint32_t user_eflags, void *entry);


process_t *mk_process(pagedir_t *pagedir, void (*entry)(void), uint32_t esp, char *description)
{
    process_t *process = kmalloc(sizeof(process_t), 1, "process_t");
    strcpy(process->description, description);
    process->description[DESC_LENGTH - 1] = 0;
    process->pid = new_pid();
    process->threads = mk_thread(process, 0, entry, esp, description);
    process->pagedir = pagedir;

    if (current_thread->process != NULL)
        process->ppid = current_thread->process->pid;
    else
        process->ppid = 0;

    return process;
}

static thread_t *mk_thread(process_t *process, pid_t tid, void (*entry)(void), uint32_t esp, char *description)
{
    thread_t *thread = kmalloc(sizeof(thread_t), 1, "thread_t");
    strcpy(thread->description, description);
    thread->description[DESC_LENGTH - 1] = 0;
    thread->next_thread = NULL;
    thread->state = RUNNING;
    thread->tid = tid;
    thread->process = process;

    // Set up kernel stack for the thread
    thread->kstack = mk_kstack(esp, 0, (void*)entry);

    return thread;
}

static process_kstack_t mk_kstack(uint32_t user_esp, uint32_t user_eflags, void *entry)
{
    process_kstack_t kstack;
    uint32_t kstack_size = 4096;
    kstack.kstack = (uint32_t)kmalloc(kstack_size, 1, "thread_t stack");

    uint32_t ds = 0x10; // -> kernel; user = 0x23;
    uint32_t cs = 0x08; // -> kernel; user = 0x1b;

    uint32_t *ksp = (uint32_t*)(kstack.kstack + kstack_size);
    kstack.ebp = (uint32_t)ksp;

    *(--ksp) = ds;
    *(--ksp) = user_esp;
    *(--ksp) = user_eflags;
    *(--ksp) = cs;
    *(--ksp) = (uint32_t)entry;

    ksp -= 8;           // general purpose registers

    *(--ksp) = ds;    // ds
    *(--ksp) = ds;    // es
    *(--ksp) = ds;    // fs
    *(--ksp) = ds;    // gs

    kstack.esp = (uint32_t)ksp;
    return kstack;
}

static pid_t new_pid()
{
    return pid_counter++;
}
