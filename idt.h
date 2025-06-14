#ifndef IDT_H
#define IDT_H
#pragma once
#include <stdint.h>
extern void irq0();
extern void irq1();

struct IDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct IDTEntry {
    uint16_t low_offset;  
    uint16_t sel;           
    uint8_t  always0;      
    uint8_t  flags;         
    uint16_t high_offset;
} __attribute__((packed));


extern void irq0(); 
extern void irq1(); 
void load_idt_register(struct IDTPtr*);
void idt_init();
void set_idt_gate(int n, uint32_t handler);
void load_idt(void* idt_ptr); 

#endif

