
#include "screen.h"
#include "idt.h"
#include "keyboard.h"
//#include "ports.h"



void kernel_main() {
  
    clear_screen();
    print("Keyboard Ready > ");

  
    idt_init();
 keyboard_input();
  
    asm volatile("sti");

   
    while (1) {
        asm volatile("hlt");
    }
}

