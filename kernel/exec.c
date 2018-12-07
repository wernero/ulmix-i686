#include "exec.h"
#include "memory/userheap.h"
#include "sched/scheduler.h"
#include "sched/task.h"

extern thread_t *current_thread;

static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv);

int sc_execve(const char *filename, char *argv[], char *envp[])
{
    // 1. locate executable and determine it's format
    // 2. parse the exutable file and load the sections into virtual address space.

    void *main_function = 0;

    paging_free_all();
    current_thread->process->nofault = 1;   // -> page fault handler won't kill us

    // COPY .TEXT, .RODATA, .DATA,
    // CLEAR .BSS

    int argc;
    void *argvp;
    unsigned long esp = (unsigned long)cpy_argv_env(argv, envp, &argc, &argvp);
    *((uint32_t*)(--esp)) = (uint32_t)argv;
    *((uint32_t*)(--esp)) = (uint32_t)argc;

    current_thread->kstack.esp = current_thread->kstack.ebp;
    kstack_init(current_thread->kstack, TYPE_USER, main_function, esp, 0x202);

    current_thread->process->nofault = 0;
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

    // create argv pointer array
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
