#include "disk.h"
#include "screen.h"
#include <stdint.h>
#include "ports.h"

#define DISK_READ_INT 0x13

#define ATA_PRIMARY_IO     0x1F0
#define ATA_PRIMARY_CTRL   0x3F6
#define ATA_CMD_READ_PIO   0x20
#define ATA_STATUS_BSY     0x80
#define ATA_STATUS_DRQ     0x08

struct disk_address_packet {
    uint8_t size;
    uint8_t reserved;
    uint16_t sectors;
    uint16_t buffer_offset;
    uint16_t buffer_segment;
    uint64_t lba;
} __attribute__((packed));

int disk_read_sector(int lba, uint8_t* buffer) {
    struct disk_address_packet dap;
    dap.size = 0x10;
    dap.reserved = 0;
    dap.sectors = 1;
    dap.buffer_offset = (uint32_t)buffer & 0x0F;
    dap.buffer_segment = ((uint32_t)buffer >> 4) & 0xFFFF;
    dap.lba = (uint64_t)lba;

    uint16_t dap_seg = ((uint32_t)&dap >> 4) & 0xFFFF;
    uint16_t dap_off = (uint32_t)&dap & 0x0F;

    int status = 0;

    __asm__ __volatile__ (
        "push %%es\n\t"
        "mov %[dap_seg], %%ax\n\t"
        "mov %%ax, %%es\n\t"
        "mov %[dap_off], %%si\n\t"
        "mov $0x42, %%ah\n\t"
        "mov $0x00, %%dl\n\t"        // HDD (0x80), floppy = 0x00
        "int $0x13\n\t"
        "jc 1f\n\t"
        "jmp 2f\n"
        "1:\n\t"
        "mov $1, %[status]\n"
        "2:\n\t"
        "pop %%es"
        : [status] "=m"(status)
        : [dap_seg] "r"(dap_seg),
          [dap_off] "r"(dap_off)
        : "ax", "si", "dx", "memory", "cc"
    );

    return status;
}

int disk_write_sector(int lba, const uint8_t* buffer) {
    int cylinder = (lba / 18) >> 1;
    int head = (lba / 18) & 1;
    int sector = (lba % 18) + 1;
    uint8_t drive = 0x00;

    uint16_t es = ((uint32_t)buffer >> 4) & 0xFFFF;
    uint16_t bx = (uint32_t)buffer & 0xF;

    int status;
    __asm__ __volatile__ (
        "mov %%ax, %%es\n\t"
        "int $0x13\n\t"
        "jc error\n\t"
        "mov $0, %[status]\n\t"
        "jmp done\n"
        "error:\n\t"
        "mov $1, %[status]\n"
        "done:"
        : [status] "=m" (status)
        : "a"(0x0301), "b"(bx), "c"(((cylinder & 0xFF) << 8) | (sector & 0x3F)),
          "d"((head << 8) | drive), "D"(es)
        : "cc", "memory"
    );

    return status;
}