gdt:
    dq 0                    ; null descriptor
.code:
    dw 0xffff               ; limit
    dw 0x0000               ; base
    db 0x00                 ; base
    db 10011010b            ; access
    db 1111b | (1100b << 4) ; limit and flags
    db 0x00                 ; base
.data:
    dw 0xffff               ; limit
    dw 0x0000               ; base
    db 0x00                 ; base
    db 10010010b            ; access
    db 1111b | (1100b << 4) ; limit and flags
    db 0x00                 ; base
.descriptor:
    dw gdt.descriptor - gdt - 1
    dd gdt

