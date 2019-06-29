OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;
    _kernel_start = .;
    .mboot  : { _ro_start = .;   *(.mboot*)                KEEP(*(.mboot))  }
    .text   : {                  *(.text*)                 KEEP(*(.text))   }
    .rodata : {                  *(.rodata*) _ro_end = .;  KEEP(*(.rodata)) }
    .data   : {                  *(.data*)                 KEEP(*(.data))   }
    .bss    : { _bss_start = .;  *(.bss*);   _bss_end = .; KEEP(*(.bss))    }
    .cdi    : { _cdi_start = .;  *(.cdi*);   _cdi_end = .; KEEP(*(.cdi))    }
    /DISCARD/ : { *(.eh_frame*); }
    _kernel_end = .;
}
