#include "exec.h"
#include "memory/userheap.h"
#include "sched/scheduler.h"
#include "sched/task.h"
#include "log.h"
#include <filesystem/fs_syscalls.h>
#include <errno.h>

extern thread_t *current_thread;

static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv);

int sc_execve(char *filename, char *argv[], char *envp[])
{
    paging_free_all();
    current_thread->process->nofault = 1;   // -> page fault handler won't kill us

    void *user_entry;
    exec_load_img(current_thread->process->pagedir, filename, &user_entry);

    int argc;
    void *argvp;
    unsigned long esp = (unsigned long)cpy_argv_env(argv, envp, &argc, &argvp);
    *((uint32_t*)(--esp)) = (uint32_t)argv;
    *((uint32_t*)(--esp)) = (uint32_t)argc;

    current_thread->process->nofault = 0;

    // modify context on kernel stack

    return 0;
}

static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv)
{
    int i;
    void *esp = (void*)GB3;

    // copy argv strings
    for (i = 0; argv[i] != NULL; i++)
    {
        esp -= strlen(argv[i]) + 1;
        memcpy(esp, argv[i], strlen(argv[i]) + 1);
        argv[i] = esp;
    }

    // create argv pointersc_execve( array
    esp -= i;
    *_argc = i;
    *_argv = esp;
    char **ptrs = (char**)esp;
    for (i = 0; ; i++)
    {
        ptrs[i] = argv[i];
        if (argv[i] == NULL)
            break;
    }

    // copy environment strings
    for (i = 0; envp[i] != NULL; i++)
    {
        esp -= strlen(envp[i]) + 1;
        memcpy(esp, envp[i], strlen(envp[i]) + 1);
        envp[i] = esp;
    }

    // create envp pointer array
    esp -= i;
    ptrs = (char**)esp;
    for (i = 0; ; i++)
    {
        ptrs[i] = argv[i];
        if (argv[i] == NULL)
            break;
    }

    return esp;
}

int exec_load_img(pagedir_t *pd, char *img_path, void **entry)
{
    int fd;
    if ((fd = sc_open(img_path, O_RDONLY)) < 0)             // on error, fd = error code
        return fd;

    // implementation of exec_load_img()

    klog(KLOG_INFO, "exec_load_img(): fd=%x *fd=%x, *node=%x, name=%s",
        fd,
        current_thread->process->files[fd],
        current_thread->process->files[fd]->direntry,
        current_thread->process->files[fd]->direntry->name
        );


    return -ENOSYS; // temporary: not implemented
}
