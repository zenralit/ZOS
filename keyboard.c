#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include <stdint.h>

#define PORT_KBD_DATA 0x60

static const char scancode_map[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',
    0, ' ', 0,
    // остальные — нули
};

void keyboard_input() {
    uint8_t scancode = inb(PORT_KBD_DATA);
    if (!(scancode & 0x80)) {
        char c = scancode_map[scancode];
        if (c) {
            put_char(c);
        }
    }
}

void keyboard_handler() {
    uint8_t sc = inb(PORT_KBD_DATA);
    if (sc & 0x80) return; // Игнор отпусканий

    char c = scancode_map[sc];
    if (c) {
        put_char(c); // Выводим только символ, без print() и прочего
    }
}



 void keyboard_init() {
    // Разрешить IRQ1 (клавиатура) на PIC
    uint8_t mask = inb(0x21);
    outb(0x21, mask & ~0x02);
}

