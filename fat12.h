#ifndef FAT12_H
#define FAT12_H
#include <stdint.h>
int read_sector(uint8_t* buffer, uint32_t lba);
void list_root_dir();
int create_file(const char* filename);
int write_sector(const uint8_t* buffer, uint32_t lba);

#endif
