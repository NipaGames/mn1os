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
    cli
    lgdt [gdt.descriptor]
    jmp CODE_SEGMENT:gdt_setup

%include "src/boot/gdt.s"

gdt_setup:
    ; prepare data segments
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, kernel_stack
    mov ebp, esp

    [extern kernel_main]
    call kernel_main

    hlt
    jmp $

%include "src/boot/kernel_stack.s"
