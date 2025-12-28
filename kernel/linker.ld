/* MetalOS Kernel Linker Script */

ENTRY(kernel_main)

SECTIONS {
    /* Kernel loaded at 1MB mark */
    . = 0x100000;
    
    .text ALIGN(4K) : {
        *(.text .text.*)
    }
    
    .rodata ALIGN(4K) : {
        *(.rodata .rodata.*)
    }
    
    .data ALIGN(4K) : {
        *(.data .data.*)
    }
    
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss .bss.*)
    }
    
    /* Discard debug info */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
