#include "idt.h"
#include "ports.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC_EOI   0x20

struct IDTEntry {
    uint16_t off_lo;
    uint16_t sel;
    uint8_t  zero;
    uint8_t  flags;
    uint16_t off_hi;
} __attribute__((packed));



static struct IDTEntry idt[256];
static struct IDTPtr   idt_ptr;

//extern void load_idt_register(struct IDTPtr*);

extern void irq1_handler();

void set_idt_gate(int n, uint32_t handler) {
    idt[n].off_lo  = handler & 0xFFFF;
    idt[n].sel     = 0x08;
    idt[n].zero    = 0;
    idt[n].flags   = 0x8E;
    idt[n].off_hi  = (handler >> 16) & 0xFFFF;
}

void idt_init() {
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (uint32_t)&idt;
    
    for (int i = 0; i < 256; i++) set_idt_gate(i, 0);
    
    set_idt_gate(0x21, (uint32_t)irq1_handler);
    
    load_idt_register(&idt_ptr);

   
    outb(PIC1_CMD, 0x11);
    outb(0xA0, 0x11);
    outb(PIC1_DATA, 0x20);
    outb(0xA1, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(0xA1, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(0xA1, 0x01);
    
    uint8_t mask = inb(PIC1_DATA);
    outb(PIC1_DATA, mask & ~(1<<1));
}

