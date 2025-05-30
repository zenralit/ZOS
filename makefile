AS   = nasm
CC   = gcc
LD   = ld

CFLAGS  = -m32 -ffreestanding -O2 -Wall
LDFLAGS = -m elf_i386

OBJS = kernel_entry.o idt.o irq1.o screen.o keyboard.o ports.o kernel.o idt_load.o irq0.o nawfs.o

all: os-image.bin

nawfs.o: nawfs.c nawfs.h ports.h screen.h keyboard.h
	$(CC) $(CFLAGS) -c nawfs.c -o $@

irq0.o: irq0.asm
	$(AS) -f elf32 $< -o $@

bootloader.bin: bootloader.asm
	$(AS) -f bin $< -o $@

kernel_entry.o: kernel_entry.asm
	$(AS) -f elf32 $< -o $@

idt.o: idt.c idt.h ports.h
	$(CC) $(CFLAGS) -c $< -o $@

irq1.o: irq1.asm
	$(AS) -f elf32 $< -o $@

screen.o: screen.c screen.h
	$(CC) $(CFLAGS) -c $< -o $@

keyboard.o: keyboard.c keyboard.h screen.h ports.h
	$(CC) $(CFLAGS) -c $< -o $@

ports.o: ports.c ports.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o $@

idt_load.o: idt_load.asm
	$(AS) -f elf32 $< -o $@

kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld -o $@ $(OBJS)

os-image.bin: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > $@

run: os-image.bin
	qemu-system-i386 -fda os-image.bin -net none 

clean:
	rm -f *.o *.bin os-image.bin

