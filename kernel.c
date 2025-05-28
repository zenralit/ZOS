#include <stdint.h>
#include "screen.h"
#include "idt.h"
#include "keyboard.h"
#include "ports.h"
#include "fat12.h"

void dummy_timer_callback() {
    
}

void kernel_main() {
  
    clear_screen();
    print("Welocome in NawOS.\n");
    print("print command >>>>\n");

  
    idt_init();
    keyboard_init();

    asm volatile("sti");
    

   
    while (1) {  
        asm volatile("hlt");
    }
}

