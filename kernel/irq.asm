section .text

global idt_write
global irq_asm_handler
global irq_asm_handler_end
global irq_asm_timer

extern irq_timer
extern irq_handler
extern exc_handler

idt_write:
    mov eax, [esp+4]
    lidt [eax]
    sti
    ret

irq_asm_timer:
    pushad
    call irq_timer
    mov al, 0x20
    out 0x20, al ; end of interrupt
    popad
    iret

%macro IRQ_HANDLER 1
    pushad
    push dword %1
    call irq_handler
    add esp, 4
    popad
    iret
%endmacro

irq_asm_handler:
    IRQ_HANDLER 0
irq_asm_handler_end:
%assign irq_no 32
%rep 16
       IRQ_HANDLER irq_no
       %assign irq_no irq_no+1
%endrep


%macro EXC_HANDLER 1
global exc%1
exc%1:
    %if (%1!=8) && (%1!=17) && (%1!=30) && (%1<10 || %1>14)
    push dword 0
    %endif
    pushad
    push dword %1
    call exc_handler
    add esp, 4
    popad
    add esp, 4
    iret
%endmacro

%assign exc_no 0
%rep 32
    EXC_HANDLER exc_no
    %assign exc_no exc_no+1
%endrep



