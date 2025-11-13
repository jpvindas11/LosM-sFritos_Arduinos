.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

    .section .multiboot
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    .section .text
    .globl _start
_start:
    cli
    mov $stack_top, %esp
    call kmain
halt:
    hlt
    jmp halt

    .section .bss
    .align 16
    .space 16384
stack_top:
