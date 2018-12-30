#include "process.h"
#include "scheduler.h"
#include <filesystem/fs_syscalls.h>
#include <kdebug.h>

static pid_t pid_counter = 0;
static pid_t new_pid(void);

extern struct dir_struct root;

process_t *mk_process(pagedir_t *pagedir,
                      thread_type_t type,
                      void (*entry)(void),
                      size_t kstack_size,
                      unsigned long esp,
                      char *description)
{
    uint32_t user_eflags = get_eflags(); // *** temporary!

    process_t *process = mk_process_struct(pagedir, type, description);
    process->threads = mk_thread(process, mk_kstack(type, (void*)entry, kstack_size, esp, user_eflags, 0), description);

    return process;
}

process_t *mk_process_struct(pagedir_t *pagedir,
                             thread_type_t type,
                             char *description)
{
    process_t *process = kmalloc(sizeof(process_t), 1, "process_t");
    strcpy(process->description, description);
    process->description[DESC_LENGTH - 1] = 0;
    process->pid = new_pid();
    process->type = type;
    process->thread_count = 0;
    process->threads = NULL;
    process->pagedir = pagedir;
    process->files[0] = NULL;
    process->working_dir = &root;

    return process;
}


void kill_process(process_t *process)
{
    scheduler_disable();

    thread_t *thr;
    for (thr = process->threads; thr != NULL; thr = thr->next_thread)
    {
        kill_thread(thr);
    }

    if (process->thread_count != 0)
    {
        klog(KLOG_WARN, "kill_process(): thread_count != 0");
    }

    for (int i = 0; process->files[i] != NULL; i++)
    {
        // close files
    }

    // release page directory

    kfree(process);
    scheduler_enable();
}

static pid_t new_pid()
{
    return pid_counter++;
}
