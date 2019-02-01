#include "syscalls.h"
#include "process.h"
#include "scheduler.h"
#include <memory/paging.h>
#include <syscall.h>
#include <kdebug.h>

extern thread_t *current_thread;

pid_t sys_fork_c(struct syscall_context_struct *context)
{
    klog(KLOG_INFO, "[%d] fork()", current_thread->process->pid);
    process_t *pold = current_thread->process;

    // create new process image
    pagedir_t *pd_new = pagedir_copy_current();
    process_t *pnew = mk_process_struct(pd_new, TYPE_USER, "forked process");

    // apply file descriptors and working directory of the old process
    pnew->working_dir = pold->working_dir;
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (pold->files[i] != NULL)
            pnew->files[i] = pold->files[i];
    }

    // new process: 'esp' is the stack pointer and also points to
    // the return address on the stack.

    struct rcontext_struct *rcontext = (struct rcontext_struct*)context;
    rcontext->eax = 0; // return value for fork() in the child process

    pnew->threads = mk_thread(pnew,
                              mk_kstack(TYPE_USER, (void*)context->eip, PAGESIZE, context->user_esp, get_eflags(), rcontext),
                              pnew->description);

    return pnew->pid; // old process gets the pid of the new one
}

void sys_exit(int status)
{
    klog(KLOG_INFO, "[%d] exit() with code %d", current_thread->process->pid, status);
    kill_process(current_thread->process);
}

pid_t sys_wait(int *wstatus)
{
    return -1;
}

pid_t sys_waitpid(pid_t pid, int *wstatus, int options)
{
    return -1;
}
