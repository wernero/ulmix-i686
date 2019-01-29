[Bits 32]

; Memory layout
; 0-1M      video memory, bootloader stuff, etc
; 1M ->     kernel binary, data, bss
; <- 6M     kernel stack
; 6M - 9M   pre-paging kernel heap
; 9M - 16M  currently unused
; 16M ->    dynamic allocation of both user and kernel stuff
;           grows towards end of physical memory

jmp kernel_start

MB_MAGIC	equ 0x1badb002
MB_FLAGS	equ 0x2 ; 0x10002 for flat binary
MB_CHECKSUM	equ -(MB_MAGIC + MB_FLAGS)
MB_LOAD_ADDR    equ 0x100000
MB_LOAD_END     equ 0x00        ; load the whole image
MB_BSS_END      equ 0x00        ; we clear BSS ourselves
MB_ENTRY_ADDR   equ 0x100000

align 4
multiboot_header:
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM
    dd (multiboot_header)
    dd MB_LOAD_ADDR
    dd MB_LOAD_END
    dd MB_BSS_END
    dd MB_ENTRY_ADDR

kernel_start:
    mov esp, 0x600000

    ; enable CPU cache
    mov eax, cr0
    and eax, 0x9FFFFFFF
    mov cr0, eax

    ; main.c: main()
    ; ebx points to struct multiboot
    push ebx
    extern main
    call   main

    cli
    hlt
