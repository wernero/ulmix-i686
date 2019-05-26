OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386)
ENTRY(_kstart)

SECTIONS
{
    . = 0x00100000;
    _kernel_beg = .;
    .mboot  : { _ro_start = .;   *(.mboot*)                KEEP(*(.mboot))  }
    .text   : {                  *(.text*)                 KEEP(*(.text))   }
    .rodata : {                  *(.rodata*) _ro_end = .;  KEEP(*(.rodata)) }
    .data   : {                  *(.data*)                 KEEP(*(.data))   }
    .bss    : { _bss_start = .;  *(.bss*);   _bss_end = .; KEEP(*(.bss))    }
    .cdi    : { _cdi_start = .;  *(.cdi*);   _cdi_end = .; KEEP(*(.cdi))    }
    /DISCARD/ : { *(.eh_frame*); }
    _kernel_end = .;
}
