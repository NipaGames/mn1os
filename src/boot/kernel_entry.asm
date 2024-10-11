[BITS 32]

section .entry
global _start
_start:
    ; prepare stack
    mov esp, kernel_stack
    mov ebp, esp

    [extern kernel_main]
    call kernel_main

    hlt
    jmp $

section .bss
align 16
resb 16384
kernel_stack:
