; обработчик IRQ1 (клавиатура)
[BITS 32]
global irq1_handler
extern keyboard_input

irq1_handler:
    pusha
    call keyboard_input
    popa
    mov al, 0x20
    out 0x20, al
    iret

