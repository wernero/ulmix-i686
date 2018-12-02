[Bits 32]

; Memory layout
; 0-1M      video memory, bootloader stuff, etc
; 1M ->     kernel binary, data, bss
; <- 6M     kernel stack
; 6M - 9M   pre-paging kernel heap
; 9M - 16M  currently unused
; 16M ->    dynamic allocation of both user and kernel stuff
;           grows towards end of physical memory

KernelStart:
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
