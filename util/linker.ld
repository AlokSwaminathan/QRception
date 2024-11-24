SECTIONS {
    . = 0x28000;  /* Starting address of the ELF */

    .text.startup : ALIGN(1) {
        *(.text.startup)
    }

    .text : ALIGN(1) {
        *(.text)
    }

    .rodata : ALIGN(1) {
        *(.rodata)
    }
}
