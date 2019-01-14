#include "syscall.h"
#include "util/util.h"
#include "filesystem/fs_syscalls.h"
#include "exec.h"
#include <sched/process.h>
#include <errno.h>
#include <log.h>

#define nop 0

/* every system call that is marked with *required is actually required
 * by the Newlib C Library and thus has to be implemented */

void *syscalls[] =
{
    nop,                // 0
    sc_exit,            // 1    exit()              *required
    sc_fork,            // 2    fork()              *required
    sc_read,            // 3    read()              *required
    sc_write,           // 4    write()             *required
    sc_open,            // 5    open()              *required
    sc_close,           // 6    close()             *required
    sc_waitpid,         // 7    waitpid()           *required
    sc_creat,           // 8    creat()
    sc_link,            // 9    link()              *required
    sc_unlink,          // 10   unlink()            *required
    sc_execve,          // 11   execve()            *required
    nop,                // 12   chdir()
    nop,                // 13   time()
    nop,                // 14   mknod()
    nop,                // 15   chmod()
    nop,                // 16   lchown16()
    nop,                // 17
    nop,                // 18   stat()              *required
    sc_lseek,           // 19   lseek()             *required
    nop,                // 20   getpid()            *required
    nop,                // 21   mount()
    nop,                // 22   oldumount()
    nop,                // 23   setuid16()
    nop,                // 24   getuid16()
    nop,                // 25   stime()
    nop,                // 26   ptrace()
    nop,                // 27   alarm()
    nop,                // 28   fstat()             * required
    nop,                // 29   pause()
    nop,                // 30   utime()
    nop,                // 31
    nop,                // 32
    nop,                // 33   access()
    nop,                // 34   nice()
    nop,                // 35
    nop,                // 36   sync()
    nop,                // 37   kill()              * required
    nop,                // 38   rename()
    nop,                // 39   mkdir()
    nop,                // 40   rmdir()
    nop,                // 41   dup()
    nop,                // 42   pipe()
    nop,                // 43   times()             *required
    nop,                // 44
    nop,                // 45   brk()               *required
    nop,                // 46   setgid16()
    nop,                // 47   getgid16()
    nop,                // 48   signal()
    nop,                // 49   geteuid16()
    nop,                // 50

    nop,                // 78	gettimeofday()      *required
};

extern thread_t *current_thread;
int syscall_handler(struct syscall_context_struct *context)
{
    klog(KLOG_DEBUG, "[%d] called system call #%d", current_thread->process->pid, context->eax);

    if (syscalls[context->eax] == NULL)
        return -ENOSYS; // not implemented

    // call actual syscall handler
    int ret;
    __asm__ (
        "push %1;"
        "push %2;"
        "push %3;"
        "push %4;"
        "push %5;"
        "call *%6;"
        "add $20, %%esp;"
        :
        "=a"(ret)
        :
        "g"(context),
        "g"(context->esi),
        "g"(context->edx),
        "g"(context->ecx),
        "g"(context->ebx),
        "g"(syscalls[context->eax])
        );

    return ret;
}
