[Bits 32]

section .text

extern syscall_table
global syscall_handler

syscall_handler:
    ; save context
    push ds
    push es
    push fs
    push gs

    push esp
    push ebp
    push esi
    push edi
    push edx
    push ecx
    push ebx

    ; get handler's address
    lea edi, [syscall_table]
    mov ebx, 4
    mul ebx
    add edi, eax
    mov edi, [edi]

    ; set segment selectors
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; call the handler
    call edi

    ; restore context
    pop ebx
    pop ecx
    pop edx
    pop edi
    pop esi
    pop ebp
    pop esp

    pop gs
    pop fs
    pop es
    pop ds

    ; return to previous task
    iret
