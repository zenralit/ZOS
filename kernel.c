#include <stdint.h>
#include "screen.h"
#include "idt.h"
#include "keyboard.h"
#include "ports.h"
#include "nawfs.h"
#include "nawstring.h"

void dummy_timer_callback() {
}

void kernel_main() {
  
    clear_screen();
    print("Welcome in NawOS.\n");
    print("print command >>>>\n");

    idt_init();
    keyboard_init();

    asm volatile("sti");

    while (1) {  
        asm volatile("hlt");
    }
}

