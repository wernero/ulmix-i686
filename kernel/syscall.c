#include "syscall.h"
#include <util/util.h>
#include <filesystem/path.h>
#include <filesystem/fs_syscalls.h>
#include <exec.h>
#include <sched/process.h>
#include <errno.h>
#include <kdebug.h>

#define nop 0

/* every system call that is marked with *required is actually required
 * by the Newlib C Library and thus has to be implemented */

void *syscalls[] =
{
    nop,                // 0
    sys_exit,           // 1    exit()              *required
    sys_fork,           // 2    fork()              *required
    sys_read,           // 3    read()              *required
    sys_write,          // 4    write()             *required
    sys_open,           // 5    open()              *required
    sys_close,          // 6    close()             *required
    sys_waitpid,        // 7    waitpid()           *required
    sys_creat,          // 8    creat()
    sys_link,           // 9    link()              *required
    sys_unlink,         // 10   unlink()            *required
    sys_execve,         // 11   execve()            *required
    sys_chdir,          // 12   chdir()
    nop,                // 13   time()
    nop,                // 14   mknod()
    nop,                // 15   chmod()
    nop,                // 16   lchown16()
    nop,                // 17
    nop,                // 18   stat()              *required
    sys_lseek,          // 19   lseek()             *required
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
    nop,                // 51
    nop,                // 52
    nop,                // 53
    sys_ioctl,          // 54   sys_ioctl()
    nop,                // 55
    nop,                // 56
    nop,                // 57
    nop,                // 58
    nop,                // 59
    nop,                // 60
    nop,                // 61
    nop,                // 62
    nop,                // 63
    nop,                // 64
    nop,                // 65
    nop,                // 66
    nop,                // 67
    nop,                // 68
    nop,                // 69
    nop,                // 70
    nop,                // 71
    nop,                // 72
    nop,                // 73
    nop,                // 74
    nop,                // 75
    nop,                // 76
    nop,                // 77
    nop,                // 78
    nop,                // 79
    nop,                // 80
    nop,                // 81
    nop,                // 82
    nop,                // 83
    nop,                // 84
    nop,                // 85
    nop,                // 86
    nop,                // 87
    nop,                // 88
    nop,                // 89
    nop,                // 90
    nop,                // 91
    nop,                // 92
    nop,                // 93
    nop,                // 94
    nop,                // 95
    nop,                // 96
    nop,                // 97
    nop,                // 98
    nop,                // 99
    nop,                // 100
    nop,                // 101
    nop,                // 102
    nop,                // 103
    nop,                // 104
    nop,                // 105
    nop,                // 106
    nop,                // 107
    nop,                // 108
    nop,                // 109
    nop,                // 110
    nop,                // 111
    nop,                // 112
    nop,                // 113
    nop,                // 114
    nop,                // 115
    nop,                // 116
    nop,                // 117
    nop,                // 118
    nop,                // 119
    nop,                // 120
    nop,                // 121
    nop,                // 122
    nop,                // 123
    nop,                // 124
    nop,                // 125
    nop,                // 126
    nop,                // 127
    nop,                // 128
    nop,                // 129
    nop,                // 130
    nop,                // 131
    nop,                // 132
    nop,                // 133
    nop,                // 134
    nop,                // 135
    nop,                // 136
    nop,                // 137
    nop,                // 138
    nop,                // 139
    nop,                // 140
    nop,                // 141
    nop,                // 142
    nop,                // 143
    nop,                // 144
    nop,                // 145
    nop,                // 146
    nop,                // 147
    nop,                // 148
    nop,                // 149
    nop,                // 150
    nop,                // 151
    nop,                // 152
    nop,                // 153
    nop,                // 154
    nop,                // 155
    nop,                // 156
    nop,                // 157
    nop,                // 158
    nop,                // 159
    nop,                // 160
    nop,                // 161
    nop,                // 162
    nop,                // 163
    nop,                // 164
    nop,                // 165
    nop,                // 166
    nop,                // 167
    nop,                // 168
    nop,                // 169
    nop,                // 170
    nop,                // 171
    nop,                // 172
    nop,                // 173
    nop,                // 174
    nop,                // 175
    nop,                // 176
    nop,                // 177
    nop,                // 178
    nop,                // 179
    nop,                // 180
    nop,                // 181
    nop,                // 182
    sys_getcwd          // 183
};

extern thread_t *current_thread;
int syscall_handler(struct syscall_context_struct *context)
{
    if (syscalls[context->eax] == NULL)
    {
        klog(KLOG_DEBUG, "pid %d: called unimplemented system call (%d)",
             current_thread->process->pid, context->eax);
        return -ENOSYS;
    }

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
