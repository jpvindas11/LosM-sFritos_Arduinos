/* filepath: /home/axel/Escritorio/LosM-sFritos_Arduinos/sistemasOperativos/Investigación/Calendarizador/linker.ld */
ENTRY(_start)

SECTIONS
{
  . = 0x00100000;

  .text ALIGN(4K) : {
    *(.multiboot)
    *(.text*)
  }

  .rodata ALIGN(4K) : {
    *(.rodata*)
  }

  .data ALIGN(4K) : {
    *(.data*)
  }

  .bss ALIGN(4K) : {
    *(COMMON)
    *(.bss*)
  }
}