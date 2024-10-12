MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
MBFLAGS  equ  MBALIGN | MEMINFO
MAGIC    equ  0x1badb002
CHECKSUM equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

section .entry
global _start
_start:
    lgdt [gdt.descriptor]

    mov esp, kernel_stack
    mov ebp, esp

    [extern kernel_main]
    call kernel_main

    hlt
    jmp $

%include "src/boot/gdt.s"
%include "src/boot/kernel_stack.s"
