#ifndef NAWFS_H
#define NAWFS_H

#include <stdint.h>
#include <stddef.h>
#define MAX_FILES 64
#define MAX_NAME_LEN 8
#define MAX_EXT_LEN 3
#define MAX_FILE_SIZE 512

typedef struct {
    char name[MAX_NAME_LEN + 1];
    char ext[MAX_EXT_LEN + 1];
    uint32_t size;
    char data[MAX_FILE_SIZE];
    uint8_t used;
} NawFile;

void fs_init();
void fs_list();
int fs_create(const char* name, const char* ext);
int fs_write(const char* name, const char* ext, const char* data);
const char* fs_read(const char* name, const char* ext);
int fs_delete(const char* name, const char* ext);

char* strstr(const char* haystack, const char* needle);
char* strncpy(char* dest, const char* src, unsigned int n);
void itoa(int value, char* str);
unsigned int strlen(const char* str);
int strcmp(const char* s1, const char* s2);
#endif
