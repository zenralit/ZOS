#include <stddef.h>
#include <stdint.h>
#ifndef KEYBOARD_H
#define KEYBOARD_H
void keyboard_input();
void keyboard_handler();
void keyboard_init();
void keyboard_handle_scancode(uint8_t scancode);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
void keyboard_handle_interrupt();
#endif

