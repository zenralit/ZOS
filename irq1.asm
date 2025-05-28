[BITS 32]
global irq1_handler
extern keyboard_handle_scancode

irq1_handler:
    pusha
    call keyboard_handle_scancode
    popa
    mov al, 0x20
    out 0x20, al
    iret
