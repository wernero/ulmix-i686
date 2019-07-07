[Bits 32]

extern exc_handler

; EXCEPTION HANDLER

%macro EXC_HANDLER 1
global exc%1
exc%1:
    %if (%1!=8) && (%1!=17) && (%1!=30) && (%1<10 || %1>14)
    push dword 0
    %else
    nop
    nop
    %endif

    push ds
    push es
    push fs
    push gs

    pushad

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword %1
    push esp
    call exc_handler
    add esp, 8

    popad

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 4
    iret
%endmacro

%assign exc_no 0
%rep 32
    EXC_HANDLER exc_no
    %assign exc_no exc_no+1
%endrep
