#include "disk.h"
#include "ports.h"
#include "screen.h"
#include <stdint.h>

#define ATA_PRIMARY_IO     0x1F0
#define ATA_PRIMARY_CTRL   0x3F6
#define ATA_CMD_READ_PIO   0x20
#define ATA_CMD_WRITE_PIO  0x30
#define ATA_STATUS_BSY     0x80
#define ATA_STATUS_DRQ     0x08

static void ata_wait() {
     uint8_t status;
    do {
        status = inb(ATA_PRIMARY_IO + 7);
    } while (status & 0x80); 
    do {
        status = inb(ATA_PRIMARY_IO + 7);
    } while (!(status & 0x08)); 
}

int disk_read_sector(int lba, uint8_t* buffer) {
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F)); 
    outb(ATA_PRIMARY_IO + 2, 1);                          
    outb(ATA_PRIMARY_IO + 3, (uint8_t)(lba));
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, ATA_CMD_READ_PIO);

    ata_wait();

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_PRIMARY_IO);
        buffer[i * 2]     = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
    }
    asm volatile("nop; nop; nop");
    return 0;
}

int disk_write_sector(int lba, const uint8_t* buffer) {
    outb(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_IO + 2, 1);                          
    outb(ATA_PRIMARY_IO + 3, (uint8_t)(lba));
    outb(ATA_PRIMARY_IO + 4, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_IO + 5, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_IO + 7, ATA_CMD_WRITE_PIO);

    ata_wait();

    for (int i = 0; i < 256; i++) {
        uint16_t data = ((uint16_t)buffer[i * 2 + 1] << 8) | buffer[i * 2];
        outw(ATA_PRIMARY_IO, data);
    }

    
    outb(ATA_PRIMARY_IO + 7, 0xE7);  
    ata_wait();

    return 0;
}