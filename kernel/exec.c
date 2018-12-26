#include "exec.h"
#include "memory/userheap.h"
#include "sched/scheduler.h"
#include "sched/task.h"
#include "log.h"
#include <elf.h>
#include <filesystem/fs_syscalls.h>
#include <errno.h>

extern thread_t *current_thread;

static void *cpy_argv_env(char *argv[], char *envp[], int *_argc, void **_argv);
static int  loadelf(char *filename, void **entry);

int kfexec(char *img_path, char *description)
{
    klog(KLOG_INFO, "kfexec(): loading elf binary");

    klog(KLOG_DEBUG, "kfexec(): creating address space");
    // 1. create virtual address space
    pagedir_t *userpd = mk_user_pagedir();

    klog(KLOG_DEBUG, "kfexec(): loading binary");
    // 2. load binary image
    int error;
    void *entry;
    current_thread->process->pagedir = userpd;
    apply_pagedir(userpd);
    if ((error = loadelf(img_path, &entry)) < 0)
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

int sc_execve(char *filename, char *argv[], char *envp[])
{
    // *** not implemented yet
    return -ENOSYS;

    // 1. free all memory occupied by the previous process
    paging_free_all();

    // 2. kill all threads

    // 3. load binary image
    int error;
    void *entry;
    if ((error = loadelf(filename, &entry)) < 0)
        return error;

    // 4. prepare stack
    int argc;
    void *argvp;
    unsigned long esp = (unsigned long)cpy_argv_env(argv, envp, &argc, &argvp);
    *((uint32_t*)(--esp)) = (uint32_t)argv;
    *((uint32_t*)(--esp)) = (uint32_t)argc;

    // 5. modify kernel stack

    return -ENOSYS;
}

static int loadelf(char *filename, void **entry)
{
    int fd;
    if ((fd = sc_open(filename, O_RDONLY)) < 0) // on error, fd = error code
        return fd;

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
            sc_lseek(fd, pht_entry.p_file, SEEK_SET);
            sc_read(fd, (void*)pht_entry.p_vaddr, pht_entry.p_filesz);
        }
    }

    // get entry address
    *entry = (void*)elf_header.entry_point;
    return SUCCESS;
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
