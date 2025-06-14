#include "keyboard.h"
#include "screen.h"
#include "ports.h"
#include "nawfs.h"
#include "disk.h"
#include <stddef.h>

#define INPUT_BUFFER_SIZE 256
#define MAX_INPUT 128
#define PORT_KBD_DATA 0x60

char input_buffer[INPUT_BUFFER_SIZE];
int input_pos = 0;

char* find_char(const char* str, char ch);
void* memcpy(void* dest, const void* src, size_t n);
void uint8_to_hex(uint8_t val, char* out);
int atoi(const char* str);
void keyboard_handle_interrupt();
int strcmp(const char* s1, const char* s2);
char* strstr(const char* haystack, const char* needle);
void reboot();
int strncmp(const char* s1, const char* s2, size_t n);



// -------- keyboard map -------- //

static const char scancode_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',
    0, ' ', 0,
};

// -------- keyboard map -------- //

void process_command(const char* input) {
    if (strcmp(input, "help") == 0) {
        print(" commands:\n");
        print("help - Show this message\n");
        print("clear - Clear screen\n");
        print("reboot - Reboot system\n");
        print("echo <text> - Print text to the screen\n");
        print("ls - Show files\n");
        print("cf <name.ext> - Create file\n");
        print("wr <text> - <name.ext> - Write to file\n");
        print("cat <name.ext> - Show file content\n");
        print("rm <name.ext> - Delete file\n");
        print("readsec <num> - Read disk sector\n");
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
    } else if (strcmp(input, "reboot") == 0) {
        reboot();
    } else if (strncmp(input, "echo ", 5) == 0) {
        print(input + 5);
        print("\n");
    } else if (strcmp(input, "ls") == 0) {
        fs_list();
    } else if (strncmp(input, "cf ", 3) == 0) {
    const char* nameext = input + 3;
    const char* dot = find_char(nameext, '.');
    if (dot && dot > nameext) {
        char name[9] = {0};
        char ext[4] = {0};
        size_t namelen = dot - nameext;
        if (namelen > 8) namelen = 8;
        memcpy(name, nameext, namelen);
        strncpy(ext, dot + 1, 3);
        ext[3] = '\0';

        if (strlen(ext) == 0 || strlen(name) == 0) {
            print("Invalid filename. Use name.ext\n");
        } else if (fs_create(name, ext) == 0) {
            print("File created\n");
        } else {
            print("Failed to create file\n");
        }
    } else {
        print("Invalid filename. Use name.ext\n");
    }
   } else if (strncmp(input, "wr ", 3) == 0) {
    const char* sep = strstr(input + 3, " - ");
    if (sep) {
        char text[128] = {0};
        char file[16] = {0};
        size_t textlen = sep - (input + 3);
        if (textlen > 127) textlen = 127;
        memcpy(text, input + 3, textlen);
        strncpy(file, sep + 3, 15);
        file[15] = '\0';

        const char* dot = find_char(file, '.');
        if (dot && dot > file) {
            char name[9] = {0};
            char ext[4] = {0};
            size_t namelen = dot - file;
            if (namelen > 8) namelen = 8;
            memcpy(name, file, namelen);
            strncpy(ext, dot + 1, 3);
            ext[3] = '\0';

            if (strlen(ext) == 0 || strlen(name) == 0) {
                print("Invalid filename. Use name.ext\n");
            } else if (fs_write(name, ext, text) == 0) {
                print("File written\n");
            } else {
                print("Failed to write file\n");
            }
        } else {
            print("Invalid filename. Use name.ext\n");
        }
    } else {
        print("Usage: wr <text> - <file>\n");
    }
    } else if (strncmp(input, "cat ", 4) == 0) {
    const char* file = input + 4;
    const char* dot = find_char(file, '.');
    if (dot && dot > file) {
        char name[9] = {0};
        char ext[4] = {0};
        size_t namelen = dot - file;
        if (namelen > 8) namelen = 8;
        memcpy(name, file, namelen);
        strncpy(ext, dot + 1, 3);
        ext[3] = '\0';

        if (strlen(ext) == 0 || strlen(name) == 0) {
            print("Invalid filename. Use name.ext\n");
        } else {
            const char* data = fs_read(name, ext);
            if (data) {
                print(data);
                print("\n");
            } else {
                print("File not found\n");
            }
        }
    } else {
        print("Invalid filename. Use name.ext\n");
    }
   } else if (strncmp(input, "rm ", 3) == 0) {
    const char* file = input + 3;
    const char* dot = find_char(file, '.');
    if (dot && dot > file) {
        char name[9] = {0};
        char ext[4] = {0};
        size_t namelen = dot - file;
        if (namelen > 8) namelen = 8;
        memcpy(name, file, namelen);
        strncpy(ext, dot + 1, 3);
        ext[3] = '\0';

        if (strlen(ext) == 0 || strlen(name) == 0) {
            print("Invalid filename. Use name.ext\n");
        } else if (fs_delete(name, ext) == 0) {
            print("File deleted\n");
        } else {
            print("Failed to delete file\n");
        }
    } else {
        print("Invalid filename. Use name.ext\n");
    }
    } else if (strncmp(input, "readsec ", 8) == 0) {
        int sec = atoi(input + 8);
        uint8_t buffer[512];
        if (disk_read_sector(sec, buffer) == 0) {
            for (int i = 0; i < 512; i++) {
                char hex[3];
                uint8_to_hex(buffer[i], hex);
                print(hex);
                print(" ");
                if ((i + 1) % 16 == 0) print("\n");
            }
        } else {
            print("Disk read error\n");
        }
    } else {
        print("Unknown command. Type 'help' for help.\n");
    }
    print("\n> ");
    input_pos = 0;
    input_buffer[0] = 0;
}


// --------- функции --------- //


void handle_input(char c) {
    if (c == '\b' && input_pos > 0) {
        input_pos--;
        input_buffer[input_pos] = 0;
        print_backspace();
    } else if (c == '\n') {
        print("\n");
        input_buffer[input_pos] = 0;
        process_command(input_buffer);
    } else if (input_pos < INPUT_BUFFER_SIZE - 1) {
        input_buffer[input_pos++] = c;
        char str[2] = {c, 0};
        print(str);
    }
}

void keyboard_handle_scancode(uint8_t scancode) {

    if (scancode & 0x80) return;  
    scancode = inb(PORT_KBD_DATA);
    if (scancode & 0x80) return;

    char c = scancode_map[scancode];
    if (!c) return;

    if (c == '\n') {
        input_buffer[input_pos] = '\0';
        print("\n");
        process_command(input_buffer);
    } else if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            put_char('\b');
        }
    } else if (input_pos < MAX_INPUT - 1) {
        input_buffer[input_pos++] = c;
        put_char(c);
    }
}

void keyboard_init() {
    print("IRQ1\n");
    uint8_t mask = inb(0x21);
    outb(0x21, mask & ~0x02);
}

char* find_char(const char* str, char ch) {
    while (*str) {
        if (*str == ch) return (char*)str;
        str++;
    }
    return NULL;
}


void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void uint8_to_hex(uint8_t val, char* out) {
    const char* hex = "0123456789ABCDEF";
    out[0] = hex[(val >> 4) & 0xF];
    out[1] = hex[val & 0xF];
    out[2] = 0;
}

int atoi(const char* str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

void keyboard_handle_interrupt() {
   
    uint8_t scancode = port_byte_in(0x60);
    keyboard_handle_scancode(scancode); 
    port_byte_out(0x20, 0x20); 
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        if ((*haystack == *needle) && (strncmp(haystack, needle, strlen(needle)) == 0)) {
            return (char*)haystack;
        }
    }
    return NULL;
}

void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}


