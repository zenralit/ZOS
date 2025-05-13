#include <stdint.h>
#include "keyboard.h"
#include "screen.h"
#include "ports.h"
#define PORT_KBD_DATA 0x60

// SCANCODE→ASCII (US)
static const char scancode_table[128] = {
  0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
 '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
 'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
 'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',
};


    
    void keyboard_init() {
    // Разрешить IRQ1 (клавиатура) на PIC
    uint8_t mask = inb(0x21);
    outb(0x21, mask & ~0x02);
}

void keyboard_input() {
    uint8_t sc = inb(PORT_KBD_DATA);

    if (sc & 0x80) {
        // Клавиша отпущена — игнорируем
        return;
    }

    char c = (sc < 128) ? scancode_table[sc] : 0;
    if (c) {
        put_char(c); // выводим символ
    }
}

