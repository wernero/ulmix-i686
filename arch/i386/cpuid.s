[Bits 32]

; int get_cpu_vendor(char *buf);
; int get_cpu_flags(unsigned int *f1, unsigned int *f2);

global get_cpu_vendor
global get_cpu_flags

is_cpuid_supported:
    pushfd
    pushfd
    xor dword [esp], (1 << 21)
    popfd
    pushfd
    pop eax
    xor eax, [esp]
    popfd
    and eax, (1 << 21)
    ret

get_cpu_vendor:
    push ebp
    mov ebp, esp
    call is_cpuid_supported
    cmp eax, 0
    je cpuid_not_supported
    push ebx
    xor eax,eax
    cpuid
    mov eax, [ebp+8]
    mov [eax], ebx
    mov [eax+4], edx
    mov [eax+8], ecx
    pop ebx
    pop ebp
    xor eax,eax
    ret

get_cpu_flags:
    mov eax, 1
    ret

cpuid_not_supported:
    mov eax, 1
    ret
