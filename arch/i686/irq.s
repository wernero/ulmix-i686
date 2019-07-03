[Bits 32]

section .text

global irq0
global irq1
extern irq_handler

%macro IRQ_HANDLER 1
    pushad
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push dword %1
    call irq_handler
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popad
    iret
%endmacro

irq0:
    IRQ_HANDLER 32

irq1:
%assign irq_no 33
%rep 15
       IRQ_HANDLER irq_no
       %assign irq_no irq_no+1
%endrep



