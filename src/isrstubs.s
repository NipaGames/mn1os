[BITS 32]

section .text

%macro ISR_STUB_ERR 1
isr_stub_%+%1:
    push %1
    jmp isr_common
%endmacro

%macro ISR_STUB_NO_ERR 1
isr_stub_%+%1:
    push 0
    push %1
    jmp isr_common
%endmacro

ISR_STUB_NO_ERR 0
ISR_STUB_NO_ERR 1
ISR_STUB_NO_ERR 2
ISR_STUB_NO_ERR 3
ISR_STUB_NO_ERR 4
ISR_STUB_NO_ERR 5
ISR_STUB_NO_ERR 6
ISR_STUB_NO_ERR 7
ISR_STUB_ERR 8
ISR_STUB_NO_ERR 9
ISR_STUB_ERR 10
ISR_STUB_ERR 11
ISR_STUB_ERR 12
ISR_STUB_ERR 13
ISR_STUB_ERR 14
ISR_STUB_NO_ERR 15
ISR_STUB_NO_ERR 16
ISR_STUB_NO_ERR 17
ISR_STUB_NO_ERR 18
ISR_STUB_NO_ERR 19
ISR_STUB_NO_ERR 20
ISR_STUB_NO_ERR 21
ISR_STUB_NO_ERR 22
ISR_STUB_NO_ERR 23
ISR_STUB_NO_ERR 24
ISR_STUB_NO_ERR 25
ISR_STUB_NO_ERR 26
ISR_STUB_NO_ERR 27
ISR_STUB_NO_ERR 28
ISR_STUB_NO_ERR 29
ISR_STUB_NO_ERR 30
ISR_STUB_NO_ERR 31
ISR_STUB_NO_ERR 32

isr_common:
    pusha
    xor eax, eax
    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    [extern isr_handle_interrupt]
    call isr_handle_interrupt
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa

    add esp, 8
    iret

global crash_me
crash_me:
    mov eax, 0
    div eax
    ret

section .data

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep 32 
    dd isr_stub_%+i
%assign i i+1 
%endrep
