#ifndef DISK_H
#define DISK_H

#include <stdint.h>

int disk_read_sector(int lba, uint8_t* buffer);

#endif

