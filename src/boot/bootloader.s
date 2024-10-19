[BITS 16]
BOOTLOADER_OFFSET equ 0x7c00
[ORG BOOTLOADER_OFFSET]

; the kernel will be moved to KERNEL_OFFSET (mainly for linker compatibility with grub multiboot)
%define KERNEL_OFFSET 0x100000

KERNEL_LOAD_SEGMENT equ 0x0000
KERNEL_LOAD_OFFSET equ BOOTLOADER_OFFSET + 512
KERNEL_LOAD_LINEAR_OFFSET equ KERNEL_LOAD_SEGMENT * 16 + KERNEL_LOAD_OFFSET
KERNEL_LOAD_SECTORS equ 40 ; each sector is 512 bytes

BOOTLOADER_STACK_OFFSET equ BOOTLOADER_OFFSET

jmp start

BOOT_DISK: db 0
TERMINAL_ROW: db 0

start:
    ; segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    ; stack
    mov ss, ax
    mov sp, BOOTLOADER_STACK_OFFSET

    mov [BOOT_DISK], dl

    ; set video mode
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    ; disable cursor
    mov ah, 0x01
    mov ch, 0x3f
    int 0x10

    ; A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; read kernel
    mov si, str.kernel_read
    call println
    mov ax, KERNEL_LOAD_SEGMENT
    mov es, ax
    mov bx, KERNEL_LOAD_OFFSET
    mov al, KERNEL_LOAD_SECTORS
    mov ch, 0x00            ; cylinder
    mov dh, 0x00            ; head
    mov cl, 0x02            ; sector
    mov dl, [BOOT_DISK]
    mov ah, 0x02
    int 0x13
    jc kernel_read_err
    cmp al, KERNEL_LOAD_SECTORS
    jne kernel_read_err

    ; load gdt and enable protected mode
    mov si, str.protected_mode
    call println
    cli
    lgdt [gdt.descriptor]
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp CODE_SEGMENT:start_protected

println:
    push ax
    push bx
    mov ah, 0x0e
    mov bh, 0x00
    mov bl, 0x07
.putc:
    lodsb
    test al, al
    jz .return
    int 0x10
    jmp .putc
.return:
    mov al, 0xd
    int 0x10
    mov al, 0xa
    int 0x10
    inc byte [TERMINAL_ROW]
    pop bx
    pop ax
    ret

str:
.kernel_read: db "Reading the kernel from disk", 0
.kernel_read_err: db "Failed reading the kernel!", 0
.protected_mode: db "Entering protected mode", 0
%defstr KERNEL_OFFSET_STR KERNEL_OFFSET
.kernel_move: db "Moving the kernel to ", KERNEL_OFFSET_STR, 0
.kernel_jump: db "Jumping to ", KERNEL_OFFSET_STR, 0

kernel_read_err:
    mov si, str.kernel_read_err
    call println
    hlt
    jmp $

%include "src/boot/gdt.s"

[BITS 32]
start_protected:
    ; prepare data segments
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
%if KERNEL_LOAD_LINEAR_OFFSET != KERNEL_OFFSET
    ; move kernel to the right offset
    mov si, str.kernel_move
    call println_protected
    mov ecx, KERNEL_LOAD_SECTORS * 512
    mov esi, KERNEL_LOAD_LINEAR_OFFSET
    mov edi, KERNEL_OFFSET
    rep
    a32 movsb
%endif
    ; done, finally
    mov si, str.kernel_jump
    call println_protected
    jmp CODE_SEGMENT:KERNEL_OFFSET

VGA_BUFFER_OFFSET equ 0xb8000
VGA_ROW_WIDTH equ 80

println_protected:
    push eax
    push ebx
    xor eax, eax
    mov al, [TERMINAL_ROW]
    mov ebx, VGA_ROW_WIDTH * 2
    mul ebx
    add eax, VGA_BUFFER_OFFSET
    mov ebx, eax
    mov ah, 0x07
.putc:
    lodsb
    test al, al
    jz .return
    mov [ebx], ax
    add ebx, 2
    jmp .putc
.return:
    inc byte [TERMINAL_ROW]
    pop ebx
    pop eax
    ret

; pad remaining
times 510 - ($ - $$) db 0
; signature
dw 0xaa55
