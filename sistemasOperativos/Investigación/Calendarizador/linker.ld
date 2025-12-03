/* filepath: /home/axel/Escritorio/LosM-sFritos_Arduinos/sistemasOperativos/Investigación/Calendarizador/linker.ld */
ENTRY(_start)

SECTIONS
{
  . = 0x00100000;

  .text ALIGN(4K) : AT(ADDR(.text)) {
    *(.multiboot)
    *(.text*)
    *(.rodata*)
  }

  .data ALIGN(4K) : AT(ADDR(.data)) {
    *(.data*)
  }

  .bss ALIGN(4K) : {
    *(COMMON)
    *(.bss*)
  }
}