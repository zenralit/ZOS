#include "disk.h"
#include "screen.h"
#include <stdint.h>

#define DISK_READ_INT 0x13

int disk_read_sector(int lba, uint8_t* buffer) {
    int cylinder = (lba / 18) >> 1;
    int head = (lba / 18) & 1;
    int sector = (lba % 18) + 1;
    uint8_t drive = 0x00;  // A:

    uint16_t es = ((uint32_t)buffer >> 4) & 0xFFFF;
    uint16_t bx = (uint32_t)buffer & 0xF;

    int status;
    __asm__ __volatile__ (
        "int $0x13\n\t"
        "jc error\n\t"
        "mov $0, %[status]\n\t"
        "jmp done\n"
        "error:\n\t"
        "mov $1, %[status]\n"
        "done:"
        : [status] "=m" (status)
        : "a"(0x0201),           // AH=0x02 (read), AL=0x01 (sectors)
          "b"(bx),
          "c"(((cylinder & 0xFF) << 8) | (sector & 0x3F)),
          "d"((head << 8) | drive),
          "D"(es)                // ES in DI (will be moved)
        : "cc", "memory"
    );

    return status;
}
