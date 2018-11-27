section .text

global idt_write
global irq_asm_handler
global irq_asm_handler_end
global irq_asm_timer

extern irq_timer
extern irq_handler

idt_write:
    mov eax, [esp+4]
    lidt [eax]
    sti
    ret

irq_asm_timer:
    pusha
    call irq_timer
    mov al, 0x20
    out 0x20, al ; end of interrupt
    popa
    iret

%macro ISR_HANDLER 1
    pusha
    push dword %1
    call irq_handler
    add esp, 4
    popa
    iret
%endmacro

irq_asm_handler:
    ISR_HANDLER 0
irq_asm_handler_end:

%assign isr_no 1
%rep 255
    ISR_HANDLER isr_no
    %assign isr_no isr_no+1
%endrep


