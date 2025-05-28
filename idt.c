#include "idt.h"
#include "ports.h"
#include "keyboard.h"
#include <stdint.h>

#define IDT_ENTRIES 256
struct IDTEntry idt[IDT_ENTRIES];
struct IDTPtr idt_ptr;

extern void load_idt_register(struct IDTPtr*);
extern void irq1_handler(); 
extern void irq0_handler(); 

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = handler & 0xFFFF;
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = (handler >> 16) & 0xFFFF;
}

void remap_pic() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

void idt_init() {
    remap_pic();

     set_idt_gate(32, (uint32_t)irq0_handler);
    set_idt_gate(33, (uint32_t)irq1_handler); 
    load_idt_register(&idt_ptr);

    idt_ptr.base = (uint32_t)&idt;
    idt_ptr.limit = sizeof(idt) - 1;
    load_idt_register(&idt_ptr);

    // Разрешаем IRQ1 (клавиатура)
    uint8_t mask = inb(0x21) & ~(1 << 1);
    outb(0x21, mask);
}

