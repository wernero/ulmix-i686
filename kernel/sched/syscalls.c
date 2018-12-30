#include "syscalls.h"
#include "process.h"
#include "scheduler.h"
#include <memory/paging.h>
#include <syscall.h>
#include <kdebug.h>

extern thread_t *current_thread;

pid_t sc_fork_c(struct syscall_context_struct *context)
{
    klog(KLOG_DEBUG, "fork() [%d]: user_esp=%x, eip=%x",
         current_thread->process->pid,
         context->user_esp,
         context->eip);
    process_t *pold = current_thread->process;

    // create new process image
    pagedir_t *pd_new = pagedir_copy(pold->pagedir);
    process_t *pnew = mk_process_struct(pd_new, TYPE_USER, "forked process");

    // apply file descriptors and working directory of the old process
    pnew->working_dir = pold->working_dir;
    // pnew->files = pold->files

    // new process: 'esp' is the stack pointer and also points to
    // the return address on the stack.
    int ret = pnew->pid; // return value for the new process = pid
    pnew->threads = mk_thread(pnew,
                              mk_kstack(TYPE_USER, (void*)context->eip, PAGESIZE, context->user_esp, get_eflags(), ret),
                              pnew->description);

    return 0; // old process gets 0
}

void sc_exit(int status)
{
    klog(KLOG_INFO, "exit() with code %d [%d]", status, current_thread->process->pid);
    kill_process(current_thread->process);
}

pid_t sc_wait(int *wstatus)
{
    return -1;
}

pid_t sc_waitpid(pid_t pid, int *wstatus, int options)
{
    return -1;
}
