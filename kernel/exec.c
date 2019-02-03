#include "exec.h"
#include <sched/task.h>
#include <sched/process.h>
#include <kdebug.h>
#include <elf.h>
#include <filesystem/fs_syscalls.h>
#include <errno.h>

extern thread_t *current_thread;
extern void irq_syscall_return(void);

#define ARG_MAX 16
#define ARG_LEN 64

char argv_buf[ARG_LEN][ARG_MAX];

static int  loadelf(int fd, void **entry);

int kfexec(char *img_path, char *description)
{
    klog(KLOG_INFO, "kfexec(): loading elf binary");
    int fd;
    if ((fd = sys_open(img_path, O_RDONLY, MODE_DEFAULT)) < 0) // on error, fd = error code
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
    unsigned long *esp = (unsigned long*)GB3;
    memset(((void*)esp) - 8192*2, 0, 8192*2);
    *(--esp) = 0;
    *(--esp) = 0;
    --esp; // Weirdly, this is necessary. Why??

    // 3. setup process structure and run
    process_t *pnew = mk_process(userpd, TYPE_USER, entry, PAGESIZE, (unsigned long)esp, description);
    if (pnew == NULL)
        return -EAGAIN;
    return SUCCESS;
}

static void cpy_argv(unsigned long *esp, int argc, char *argv[], void **argvp)
{
    // copy argument strings onto the stack
    int i, len;
    char *esp_str = (char*)esp;
    for (i = 0; i < argc; i++)
    {
        len = strlen(argv_buf[i]) + 1;
        esp_str -= len;
        memcpy(esp_str, argv_buf[i], len);
        argv[i] = esp_str;
    }

    // align the stack
    if ((unsigned long)esp_str % sizeof(unsigned long) != 0)
        esp_str -= ((unsigned long)esp_str % sizeof(unsigned long));
    esp = (unsigned long *)esp_str;

    // copy pointer array into the stack
    for (i = argc - 1; i >= 0; i--)
    {
        *(--esp) = (unsigned long)argv[i];
    }

    *argvp = (void*)esp;
}

int sys_execve(char *filename, char *argv[], char *envp[])
{
    klog(KLOG_INFO, "[%d] execve(): file=%s", current_thread->process->tgid, filename);

    int argc = 0;
    if (argv != NULL)
    {
        for (argc = 0; argv[argc] != NULL && argc < ARG_MAX; argc++)
            strcpy(argv_buf[argc], argv[argc]);
    }

    // 1. check wheter the binary image exists and can be read
    int fd;
    if ((fd = sys_open(filename, O_RDONLY, MODE_DEFAULT)) < 0) // on error, fd = error code
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
    void *argvp;
    unsigned long *esp = (unsigned long*)GB3;
    memset(((void*)esp) - 8192*2, 0, 8192*2);
    cpy_argv(esp, argc, argv, &argvp);
    *(--esp) = (unsigned long)argvp;
    *(--esp) = argc;
    esp--; // Necessary, but why?

    /*
    unsigned long esp = (unsigned long)cpy_argv_env(argv, envp, &argc, &argvp);
    *((uint32_t*)(--esp)) = (uint32_t)argv;
    *((uint32_t*)(--esp)) = (uint32_t)argc;*/

    // 6. create new kernel stack
    current_thread->kstack.esp = current_thread->kstack.ebp;
    current_thread->kstack = kstack_init(current_thread->kstack, TYPE_USER, entry, (unsigned long)esp, get_eflags(), NULL);

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
