section .text

global sys_fork
extern sys_fork_c

sys_fork:
    push dword [esp+20]
    call sys_fork_c
    add esp, 4
    ret
