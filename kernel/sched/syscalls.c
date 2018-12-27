#include "syscalls.h"
#include <memory/paging.h>
#include <kdebug.h>

extern thread_t *current_thread;

pid_t sc_fork_c(unsigned long esp)
{
    process_t *pold = current_thread->process;

    // !!! only works from kernel yet
    pagedir_t *pd_new = pagedir_copy(pold->pagedir);

    process_t *pnew = mk_process_struct(pd_new, TYPE_USER, "forked process");
    pnew->working_dir = pold->working_dir;

    // new process: 'esp' is the stack pointer and also points to
    // the return address on the stack.
    int ret = pnew->pid; // return value for the new process = pid
    pnew->threads = mk_thread(pnew,
                              mk_kstack(TYPE_USER, (void*)esp, PAGESIZE, esp, get_eflags(), ret),
                              pnew->description);

    return 0; // old process gets 0
}

void sc_exit(int status)
{
    klog(KLOG_INFO, "exit() with code %d", status);
    cli();
    hlt();
}

pid_t sc_wait(int *wstatus)
{
    return -1;
}

pid_t sc_waitpid(pid_t pid, int *wstatus, int options)
{
    return -1;
}
