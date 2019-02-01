#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"

struct _process_t
{
    pid_t               pid;
    thread_type_t       type;
    pagedir_t*          pagedir;
    thread_t*           threads;
    int                 thread_count;
    int                 nofault;
    struct dir_struct   *working_dir;
    struct file_struct  *files[MAX_FILES];
    char                description[DESC_LENGTH];
};

process_t * mk_process(pagedir_t *pagedir,
                       thread_type_t type,
                       void (*entry)(void),
                       size_t kstack_size,
                       unsigned long esp,
                       char *description);

process_t * mk_process_struct(pagedir_t *pagedir,
                              thread_type_t type,
                              char *description);

void kill_process(process_t *process);

pid_t sys_getpid(void);


#endif // PROCESS_H
