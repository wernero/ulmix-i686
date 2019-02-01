#ifndef PROCESS_H
#define PROCESS_H

#include "task.h"

typedef enum
{
    P_RUNNING,
    P_EXITED
} pstate_t;

struct _process_t
{
    process_t *         parent;
    process_t *         children;
    process_t *         next_child;
    pid_t               tgid;
    thread_type_t       type;
    pagedir_t*          pagedir;
    thread_t*           threads;
    int                 thread_count;
    pstate_t            state;
    thread_t *          wait_thread;
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

void process_add_child(process_t *proc, process_t *child);
void kill_process(process_t *process);

pid_t sys_getpid(void);
pid_t sys_getppid(void);


#endif // PROCESS_H
