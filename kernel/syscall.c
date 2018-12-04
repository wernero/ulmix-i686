#include "syscall.h"
#include "sched/syscalls.h"
#include "exec.h"
#include "filesystem/filesystem.h"

void *syscalls[] =
{
    nop,                // 0
    sc_exit,            // 1    exit()
    sc_fork,            // 2    fork()
    sc_read,            // 3    read()
    sc_write,           // 4    write()
    sc_open,            // 5    open()
    sc_close,           // 6    close()
    sc_waitpid,         // 7    waitpid()
    sc_creat,           // 8    creat()
    sc_link,            // 9    link()
    sc_unlink,          // 10   unlink()
    sc_execve,          // 11   execve()
    nop,                // 12   chdir()
    nop,                // 13   time()
    nop,                // 14   mknod()
    nop,                // 15   chmod()
    nop,                // 16   lchown16()
    nop,                // 17
    nop,                // 18   stat()
    sc_lseek,           // 19   lseek()
    nop,                // 20   getpid()
    nop,                // 21   mount()
    nop,                // 22   oldumount()
    nop,                // 23   setuid16()
    nop,                // 24   getuid16()
    nop,                // 25   stime()
    nop,                // 26   ptrace()
    nop,                // 27   alarm()
    nop,                // 28   fstat()
    nop,                // 29   pause()
    nop,                // 30   utime()
    nop,                // 31
    nop,                // 32
    nop,                // 33   access()
    nop,                // 34   nice()
    nop,                // 35
    nop,                // 36   sync()
    nop,                // 37   kill()
    nop,                // 38   rename()
    nop,                // 39   mkdir()
    nop,                // 40   rmdir()
    nop,                // 41   dup()
    nop,                // 42   pipe()
    nop,                // 43   times()
    nop,                // 44
    nop,                // 45   brk()
    nop,                // 46   setgid16()
    nop,                // 47   getgid16()
    nop,                // 48   signal()
    nop,                // 49   geteuid16()
    nop,                // 50
};
