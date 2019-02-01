#ifndef WAIT_H
#define WAIT_H

pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);

// int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

#endif // WAIT_H
