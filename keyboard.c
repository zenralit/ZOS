#include "keyboard.h"
#include "screen.h"
#include "ports.h"
#include "fat12.h"  
#include <string.h>

#define MAX_INPUT 128
#define PORT_KBD_DATA 0x60

static char input_buffer[MAX_INPUT];
static uint8_t input_pos = 0;

static const char scancode_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',
    0, ' ', 0,
};

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
    }
    return 0;
}

void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void process_command(const char* input) {
    if (strcmp(input, "help") == 0) {
        print(" commands:\n");
        print("help - Show this message\n");
        print("clear - Clear screen\n");
        print("reboot - Reboot system\n");
        print("echo <text> - Print text to the screen\n");
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strncmp(input, "echo ", 5) == 0) {
        print(input + 5);  
        print("\n");
    } else if (strcmp(input, "ls") == 0) {
    list_root_dir();
    } else if (strncmp(input, "touch ", 6) == 0) {
    create_file(input + 6);
    } else {
        print("Unknown command. Type 'help' for help.\n");
    }
    print("\n> ");
    input_pos = 0;
    input_buffer[0] = 0;
}

void keyboard_handle_scancode() {
    uint8_t scancode = inb(PORT_KBD_DATA);
    if (scancode & 0x80) return;

    char c = scancode_map[scancode];
    if (!c) return;

    if (c == '\n') {
        input_buffer[input_pos] = '\0';
        print("\n");
        process_command(input_buffer);
    } else if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            put_char('\b');
        }
    } else if (input_pos < MAX_INPUT - 1) {
        input_buffer[input_pos++] = c;
        put_char(c);
    }
}

void keyboard_init() {
    uint8_t mask = inb(0x21);
    outb(0x21, mask & ~0x02);
}
