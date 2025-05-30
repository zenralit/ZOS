#include "nawfs.h"
#include "screen.h" 

#include "keyboard.h"

static NawFile files[MAX_FILES];


char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && *h == *n) {
            h++;
            n++;
        }
        if (!*n) return (char*)haystack;
    }
    return 0;
}

char* strncpy(char* dest, const char* src, unsigned int n) {
    unsigned int i;
    for (i = 0; i < n && src[i]; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}



void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].used = 0;
    }
}

void fs_list() {
    print("Files:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used) {
            print("  ");
            print(files[i].name);
            print(".");
            print(files[i].ext);
            print(" (size: ");
            char size[10];
            itoa(files[i].size, size);
            print(size);
            print(")\n");
        }
    }
}

int fs_create(const char* name, const char* ext) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].used) {
            strncpy(files[i].name, name, MAX_NAME_LEN);
            files[i].name[MAX_NAME_LEN] = '\0';
            strncpy(files[i].ext, ext, MAX_EXT_LEN);
            files[i].ext[MAX_EXT_LEN] = '\0';
            files[i].size = 0;
            files[i].used = 1;
            return 0;
        }
    }
    return -1;
}

int fs_write(const char* name, const char* ext, const char* data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0 && strcmp(files[i].ext, ext) == 0) {
            strncpy(files[i].data, data, MAX_FILE_SIZE);
            files[i].size = strlen(data);
            return 0;
        }
    }
    return -1;
}

const char* fs_read(const char* name, const char* ext) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0 && strcmp(files[i].ext, ext) == 0) {
            return files[i].data;
        }
    }
    return NULL;
}

int fs_delete(const char* name, const char* ext) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && strcmp(files[i].name, name) == 0 && strcmp(files[i].ext, ext) == 0) {
            files[i].used = 0;
            return 0;
        }
    }
    return -1;
}

void itoa(int value, char* str) {
    char* p = str;
    char* p1, *p2;
    int digits = 0;

    if (value == 0) {
        *p++ = '0';
        *p = '\0';
        return;
    }

    if (value < 0) {
        *p++ = '-';
        value = -value;
    }

    p1 = p;

    while (value) {
        *p++ = '0' + (value % 10);
        value /= 10;
        digits++;
    }
    *p = '\0';

    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}
unsigned int strlen(const char* str) {
    unsigned int len = 0;
    while (str[len]) len++;
    return len;
}
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
