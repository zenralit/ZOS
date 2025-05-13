#include "screen.h"
#include "ports.h"
#include <stdint.h>

#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

static uint16_t* const VIDEO_MEMORY = (uint16_t*)0xB8000;
static int cursor_offset = 0;

static void update_cursor() {
    int offset = cursor_offset / 2;
    outb(0x3D4, 14);
    outb(0x3D5, (uint8_t)(offset >> 8));
    outb(0x3D4, 15);
    outb(0x3D5, (uint8_t)(offset & 0xFF));
}

void clear_screen() {
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        VIDEO_MEMORY[i] = (WHITE_ON_BLACK << 8) | ' ';
    }
    cursor_offset = 0;
    update_cursor();
}

void put_char(char c) {
    if (c == '\n') {
        cursor_offset += (MAX_COLS * 2) - (cursor_offset % (MAX_COLS * 2));
    } else if (c == '\b') {
        if (cursor_offset >= 2) {
            cursor_offset -= 2;
            VIDEO_MEMORY[cursor_offset / 2] = (WHITE_ON_BLACK << 8) | ' ';
        }
    } else {
        VIDEO_MEMORY[cursor_offset / 2] = (WHITE_ON_BLACK << 8) | c;
        cursor_offset += 2;
    }

    if (cursor_offset >= MAX_ROWS * MAX_COLS * 2) {
        clear_screen();  // простая реализация: очищаем экран при переполнении
    }

    update_cursor();
}

void print(const char* str) {
    while (*str) {
        put_char(*str++);
    }
}

