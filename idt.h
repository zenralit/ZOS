#ifndef IDT_H
#define IDT_H

#include <stdint.h>
struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void load_idt_register(struct IDTPtr*);
void idt_init();
void set_idt_gate(int n, uint32_t handler);
void load_idt(void* idt_ptr); 

#endif

