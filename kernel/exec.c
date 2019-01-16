#include "exec.h"
#include "memory/userheap.h"
#include <sched/task.h>
#include <sched/process.h>
#include <kdebug.h>
#include <elf.h>
#include <filesystem/fs_syscalls.h>
#include <errno.h>

extern thread_t *current_thread;
extern void irq_syscall_return(void);

//static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv);
static int  loadelf(int fd, void **entry);

int kfexec(char *img_path, char *description)
{
    klog(KLOG_INFO, "kfexec(): loading elf binary");
    int fd;
    if ((fd = sys_open(img_path, O_RDONLY)) < 0) // on error, fd = error code
        return fd;

    klog(KLOG_DEBUG, "kfexec(): creating address space");
    // 1. create virtual address space
    pagedir_t *userpd = mk_user_pagedir();

    klog(KLOG_DEBUG, "kfexec(): loading binary");
    // 2. load binary image
    int error;
    void *entry;
    current_thread->process->pagedir = userpd;
    apply_pagedir(userpd);
    if ((error = loadelf(fd, &entry)) < 0)
        return error;

    // 3. setup stack
    unsigned long ebp, esp = GB3;
    ebp = GB3 - 4096;
    memset((void*)ebp, 0, 4096);

    // 3. setup process structure and run
    process_t *pnew = mk_process(userpd, TYPE_USER, entry, PAGESIZE, esp, description);
    if (pnew == NULL)
        return -EAGAIN;
    return SUCCESS;
}

int sys_execve(char *filename, char *argv[], char *envp[])
{
    klog(KLOG_INFO, "[%d] execve(): file=%s", current_thread->process->pid, filename);

    // 1. check wheter the binary image exists and can be read
    int fd;
    if ((fd = sys_open(filename, O_RDONLY)) < 0) // on error, fd = error code
        return fd;

    // 2. create virtual address space
    delete_pagedir(current_thread->process->pagedir);
    current_thread->process->pagedir = mk_user_pagedir();
    apply_pagedir(current_thread->process->pagedir);

    // 3. kill all threads

    // 4. load binary image
    int error;
    void *entry;
    if ((error = loadelf(fd, &entry)) < 0)
        return error;

    // 5. setup stack
    unsigned long ebp, esp = GB3;
    ebp = GB3 - 4096;
    memset((void*)ebp, 0, 4096);

    /*int argc;
    void *argvp;
    unsigned long esp = (unsigned long)cpy_argv_env(argv, envp, &argc, &argvp);
    *((uint32_t*)(--esp)) = (uint32_t)argv;
    *((uint32_t*)(--esp)) = (uint32_t)argc;*/

    // 6. create new kernel stack
    current_thread->kstack.esp = current_thread->kstack.ebp;
    current_thread->kstack = kstack_init(current_thread->kstack, TYPE_USER, entry, esp, get_eflags(), NULL);

    // 7. Return from System call handler
    __asm__("mov %0, %%esp;"
            "jmp irq_syscall_return"
            : :
            "g"(current_thread->kstack.esp));

    return SUCCESS;
}

static int loadelf(int fd, void **entry)
{
    // get ELF header
    int error;
    struct elf_header_struct elf_header;
    if ((error = elf_read_header(fd, &elf_header)) < 0)
        return error;

    // go through program header table and load all segments with type == 1
    struct elf_pht_entry_struct pht_entry;
    for (int i = 0; i < elf_header.pht_entries; i++)
    {
        if ((error = elf_get_pht_entry(fd, i, &elf_header, &pht_entry)) < 0)
            return error;

        // only load segment into memory when type == 1
        if (pht_entry.type == 1)
        {
            klog(KLOG_DEBUG, "loadelf(): PHT #%d: loading %S at location 0x%x",
                 i,
                 pht_entry.p_filesz,
                 pht_entry.p_vaddr);

            // check for invalid values
            if (pht_entry.p_filesz > pht_entry.p_memsz ||
                    pht_entry.p_vaddr < MB16 ||
                    (pht_entry.p_vaddr + pht_entry.p_memsz) >= GB3)
                return -ENOEXEC; // exec format error

            // clear region to zero and load
            memset((void*)(pht_entry.p_vaddr + pht_entry.p_filesz), 0, pht_entry.p_memsz - pht_entry.p_filesz);
            sys_lseek(fd, pht_entry.p_file, SEEK_SET);
            sys_read(fd, (void*)pht_entry.p_vaddr, pht_entry.p_filesz);
        }
    }

    // get entry address
    *entry = (void*)elf_header.entry_point;
    return SUCCESS;
}

/*static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv)
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

    // create argv pointersys_execve( array
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
}*/
