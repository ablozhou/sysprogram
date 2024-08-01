[BITS 16]
[ORG 0x7C00]

start:
    ; 设置段寄存器
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; 打印 "Hello, World!"
    mov si, hello_message

print_loop:
    lodsb
    cmp al, 0
    je print_done
    mov ah, 0x0E
    int 0x10
    jmp print_loop

print_done:
    ; 无限循环
    cli
    hlt

hello_message db 'Hello, World!', 0

times 510 - ($ - $$) db 0
dw 0xAA55
