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

int exec_load_img(char *filename, void **entry);

int sc_execve(char *filename, char *argv[], char *envp[])
{
    // 1. free all memory occupied by the previous process
    paging_free_all();

    // 2. kill all threads

    // 3. load executable
    void *entry_point;
    exec_load_img(filename, &entry_point);

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


int exec_load_img(char *filename, void **entry)
{
    return -ENOSYS;
}

int kfexec(char *img_path)
{
    klog(KLOG_INFO, "exec_load_img(): loading elf binary");

    int fd;
    if ((fd = sc_open(img_path, O_RDONLY)) < 0)             // on error, fd = error code
        return fd;


    char *file_load_buffer = kmalloc(current_thread->process->files[fd]->direntry->size, 1, "file exec buffer");
    sc_lseek(fd, 480, SEEK_SET);
    sc_read(fd,file_load_buffer, current_thread->process->files[fd]->direntry->size);
    hexdump(KLOG_INFO, file_load_buffer, 0x80);

    int error;
    struct elf_header_struct *elf_header;
    if ((error = elf_read_header(fd, &elf_header)) < 0)
        return error;

    return 0;

    current_thread->process->nofault = 1;

    // go through segments (not sections!)
    struct elf_pht_entry_struct pht_entry;
    for (int i = 0; i < elf_header->pht_entries; i++)
    {
        if ((error = elf_get_pht_entry(fd, i, elf_header, &pht_entry)) < 0)
            return error;


        klog(KLOG_INFO, "segment: type=%d, flags=%x", pht_entry.type, pht_entry.flags);

        // only load segment into memory when type == 1
        if (pht_entry.type == 1)
        {
            // TODO: apply read/write/execute flags on pages

            //memset((void*)pht_entry.p_vaddr, 0, pht_entry.p_memsz);
            //sc_lseek(fd, pht_entry.p_file, SEEK_SET);
            //sc_read(fd, (void*)pht_entry.p_vaddr, pht_entry.p_filesz);
        }
    }

    current_thread->process->nofault = 0;
    void *entry = (void*)elf_header->entry_point;
    kfree(elf_header);

    klog(KLOG_INFO, "exec_load_img(): entry=%x, fd=%d *fd=%x, *node=%x, name=%s",
        (unsigned long)entry,
        fd,
        current_thread->process->files[fd],
        current_thread->process->files[fd]->direntry,
        current_thread->process->files[fd]->direntry->name
        );

    return -ENOSYS; // temporary: not implemented
}
