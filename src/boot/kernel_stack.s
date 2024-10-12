; reserve 2MB for the stack
section .bss
align 16
resb 0x200000
kernel_stack:
