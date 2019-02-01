#include "syscalls.h"
#include "process.h"
#include "scheduler.h"
#include <memory/paging.h>
#include <syscall.h>
#include <kdebug.h>

extern thread_t *current_thread;

pid_t sys_fork_c(struct syscall_context_struct *context)
{
    klog(KLOG_INFO, "[%d] fork()", current_thread->process->tgid);
    process_t *pold = current_thread->process;

    // create new process image
    pagedir_t *pd_new = pagedir_copy_current();
    process_t *pnew = mk_process_struct(pd_new, TYPE_USER, "forked process");
    process_add_child(pold, pnew);

    // apply file descriptors and working directory of the old process
    pnew->working_dir = pold->working_dir;
    pnew->parent = pold;
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

    return pnew->tgid; // old process gets the pid of the new one
}

void sys_exit(int status)
{
    klog(KLOG_INFO, "[%d] exit() with code %d", current_thread->process->tgid, status);
    kill_process(current_thread->process);
}

pid_t sys_waitpid(pid_t pid, int *wstatus, int options)
{
    if (pid <= 0)
        return -ENOSYS;

    process_t *wproc;
    for (wproc = current_thread->process->children; wproc != NULL; wproc = wproc->next_child)
    {
        if (wproc->tgid == pid)
        {
            if (wproc->state == P_RUNNING)
            {
                wproc->wait_thread = current_thread;
                scheduler_block(current_thread);
            }

            return wproc->tgid;
        }
    }
    return -ECHILD;
}
