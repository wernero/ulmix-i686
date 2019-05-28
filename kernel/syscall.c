#include "syscall.h"
#include <util/util.h>
#include <errno.h>
#include <uname.h>

#define nop 0

void *syscalls[] =
{
    nop,                // 0
nop, //    sys_exit,           // 1    terminate program
nop, //    sys_fork,           // 2    copy current process
nop, //    sys_read,           // 3    read from file descriptor
nop, //    sys_write,          // 4    write into file descriptor
nop, //    sys_open,           // 5    open file or device
nop, //    sys_close,          // 6    close file descriptor
nop, //    sys_waitpid,        // 7    wait for an event to happen
nop, //    sys_creat,          // 8    = open(path, O_WRONLY, O_TRUNC)
nop, //    sys_link,           // 9    make a hard link to a file
nop, //    sys_unlink,         // 10   delete a file
    nop, //sys_execve,         // 11   replace process with a new one
nop, //    sys_chdir,          // 12   change working directory
    nop,                // 13   time()
    nop,                // 14   mknod()
    nop,                // 15   chmod()
    nop,                // 16   lchown16()
nop, //    sys_kdebug,         // 17   kernel debugging features
    nop,                // 18   stat()              *required
nop, //    sys_lseek,          // 19   set seek offset in a file
    nop, //sys_getpid,         // 20   get process id
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
nop, //    sys_ioctl,          // 54   send I/O command
    nop,                // 55
    nop,                // 56
    nop,                // 57
    nop,                // 58
    sys_uname,          // 59   get kernel info     = 109 = 122
    nop,                // 60
    nop,                // 61
    nop,                // 62
    nop,                // 63
    nop, //sys_getppid,        // 64   get parent process id
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
    sys_uname,          // 109 = 59 = 122 for compatibility
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
    sys_uname,          // 122 = 109 = 59 for compatibility
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
nop, //    sys_getcwd          // 183
};

int syscall_handler(struct syscall_context_struct *context)
{
    if (syscalls[context->eax] == NULL)
    {
        //klog(KLOG_DEBUG, "pid %d: called unimplemented system call (%d)",
          //   current_thread->process->tgid, context->eax);
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
