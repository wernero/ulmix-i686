section .text

global sc_fork
extern sc_fork_c

sc_fork:
    push dword [esp+20]
    call sc_fork_c
    add esp, 4
    ret
