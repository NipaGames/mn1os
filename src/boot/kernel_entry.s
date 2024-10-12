[BITS 32]

section .entry
global _start
_start:
    mov esp, kernel_stack
    mov ebp, esp

    [extern kernel_main]
    call kernel_main

    hlt
    jmp $

%include "src/boot/kernel_stack.s"
