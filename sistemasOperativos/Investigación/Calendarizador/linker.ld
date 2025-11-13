/* linker.ld — enlaza en formato ELF 32-bit */
ENTRY(_start)

SECTIONS
{
  . = 0x00100000;         /* cargar kernel en 1MB */

  .multiboot : { *(.multiboot) }

  .text : {
    *(.text*)
  }

  .rodata : { *(.rodata*) }

  .data : { *(.data*) }

  .bss : {
    *(.bss*)
    *(COMMON)
  }
}