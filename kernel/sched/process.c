#include "process.h"
#include "scheduler.h"
#include <filesystem/fs_syscalls.h>
#include <kdebug.h>

static pid_t pid_counter = 0;
static pid_t new_pid(void);

extern struct dir_struct root;
extern thread_t *current_thread;

process_t *mk_process(pagedir_t *pagedir,
                      thread_type_t type,
                      void (*entry)(void),
                      size_t kstack_size,
                      unsigned long esp,
                      char *description)
{
    uint32_t user_eflags = get_eflags(); // *** temporary!

    process_t *process = mk_process_struct(pagedir, type, description);
    process->threads = mk_thread(process, mk_kstack(type, (void*)entry, kstack_size, esp, user_eflags, NULL), description);

    return process;
}

process_t *mk_process_struct(pagedir_t *pagedir,
                             thread_type_t type,
                             char *description)
{
    process_t *process = kmalloc(sizeof(process_t), 1, "process_t");
    strcpy(process->description, description);
    process->description[DESC_LENGTH - 1] = 0;
    process->tgid = new_pid();
    process->type = type;
    process->thread_count = 0;
    process->threads = NULL;
    process->pagedir = pagedir;
    process->files[0] = NULL;
    process->working_dir = &root;
    process->parent = NULL;
    process->children = NULL;
    process->next_child = NULL;
    process->state = P_RUNNING;
    process->wait_thread = NULL;
    return process;
}

void process_add_child(process_t *proc, process_t *child)
{
    if (proc->children != NULL)
    {
        child->next_child = proc->children;
    }

    proc->children = child;
}


void kill_process(process_t *process)
{
    if (process->wait_thread != NULL)
    {
        scheduler_unblock(process->wait_thread);
        process->wait_thread = NULL;
    }

    scheduler_disable();

    thread_t *thr;
    int is_running = 0;
    for (thr = process->threads; thr != NULL; thr = thr->next_thread)
    {
        scheduler_remove(thr);

        if (current_thread == thr)
            is_running = 1;
    }

    // release page directory
    // close all files
    for (int i = 0; process->files[i] != NULL; i++)
    {
        // close file
    }

    scheduler_enable();
    if (is_running)
        scheduler_force();
}

static pid_t new_pid()
{
    return pid_counter++;
}

pid_t sys_getpid(void)
{
    return current_thread->process->tgid;
}

pid_t sys_getppid(void)
{
    if (current_thread->process->parent == NULL)
        return 0;
    return current_thread->process->parent->tgid;
}
