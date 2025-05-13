#include <stdint.h>
#include "screen.h"
#include "idt.h"
#include "keyboard.h"
//#include "ports.h"



void kernel_main() {
  
    clear_screen();
    print("Keyboard Ready > ");

  
    idt_init();
    keyboard_init();

    asm volatile("sti");
    

   
    while (1) {  
        asm volatile("hlt");
    }
}

