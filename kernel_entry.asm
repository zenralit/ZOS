[BITS 32]
global kernel_entry
extern kernel_main

kernel_entry:
    call kernel_main
.loop:
    hlt
    jmp .loop

