section .text

global gdt_write
global tss_write
global paging_enable

gdt_write:
    mov eax, [esp+4]
    lgdt [eax]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.write
.write:
    ret


tss_write:
    mov ax, 0x2b
    ltr ax
    ret

paging_enable:
    push ebp
    mov ebp, esp
    mov eax, cr0
    or eax, 0x80010000
    mov cr0, eax
    mov esp, ebp
    pop ebp
    ret
