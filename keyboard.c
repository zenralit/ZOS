#include "keyboard.h"
#include "screen.h"
#include "ports.h"
#include "nawfs.h"  
#include <string.h>

#define MAX_INPUT 128
#define PORT_KBD_DATA 0x60

static char input_buffer[MAX_INPUT];
static uint8_t input_pos = 0;

static const char scancode_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*',
    0, ' ', 0,
};

// int strcmp(const char* s1, const char* s2) {
//     while (*s1 && (*s1 == *s2)) {
//         s1++;
//         s2++;
//     }
//     return *(const unsigned char*)s1 - *(const unsigned char*)s2;
// }

int strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
    }
    return 0;
}

void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void process_command(const char* input) {
    if (strcmp(input, "help") == 0) {
        print(" commands:\n");
        print("help - Show this message\n");
        print("clear - Clear screen\n");
        print("reboot - Reboot system\n");
        print("echo <text> - Print text to the screen\n");
        print("ls - Show files\n");
        print("cf <name.***> - Create file\n");
        print("cat <name.***> - Show file content\n");
        print("df <name.***> - Delete file\n");
        print("wr <text> - <name.***> - Write to file\n");
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
    const char* fname = input + 3;
        char name[9] = {0}, ext[4] = {0};
        int i = 0;
        while (*fname && *fname != '.' && i < 8) name[i++] = *fname++;
        name[i] = '\0';
        if (*fname == '.') fname++;
        i = 0;
        while (*fname && i < 3) ext[i++] = *fname++;
        ext[i] = '\0';

        if (fs_create(name, ext) == 0)
            print("File created\n");
        else
            print("Create failed\n");
    } 
    else if (strncmp(input, "cat ", 4) == 0) {
        const char* fname = input + 4;
        char name[9] = {0}, ext[4] = {0};
        int i = 0;
        while (*fname && *fname != '.' && i < 8) name[i++] = *fname++;
        name[i] = '\0';
        if (*fname == '.') fname++;
        i = 0;
        while (*fname && i < 3) ext[i++] = *fname++;
        ext[i] = '\0';

        const char* data = fs_read(name, ext);
        if (data)
            print(data);
        else
            print("Not found\n");
        print("\n");
    }
    else if (strncmp(input, "df ", 3) == 0) {
        const char* fname = input + 3;
        char name[9] = {0}, ext[4] = {0};
        int i = 0;
        while (*fname && *fname != '.' && i < 8) name[i++] = *fname++;
        name[i] = '\0';
        if (*fname == '.') fname++;
        i = 0;
        while (*fname && i < 3) ext[i++] = *fname++;
        ext[i] = '\0';

        if (fs_delete(name, ext) == 0)
            print("Deleted\n");
        else
            print("Delete failed\n");
    }else if (strncmp(input, "wr ", 3) == 0) {
        const char* args = input + 3;
        const char* dash = strstr(args, " - ");
        if (!dash) {
            print("Syntax: wr <text> - <name.***>\n");
        } else {
            int text_len = dash - args;
            char content[128] = {0};
            char fname[13] = {0};
            strncpy(content, args, text_len);
            content[text_len] = '\0';

            const char* fstart = dash + 3;
            int i = 0;
            while (*fstart && i < 12) fname[i++] = *fstart++;
            fname[i] = '\0';

            char name[9] = {0}, ext[4] = {0};
            i = 0;
            const char* p = fname;
            while (*p && *p != '.' && i < 8) name[i++] = *p++;
            name[i] = '\0';
            if (*p == '.') p++;
            i = 0;
            while (*p && i < 3) ext[i++] = *p++;
            ext[i] = '\0';

            if (fs_write(name, ext, content) == 0)
                print("Written\n");
            else
                print("Write failed\n");
        } } else {
        print("Unknown command. Type 'help' for help.\n");
    }
    print("\n> ");
    input_pos = 0;
    input_buffer[0] = 0;
}

void keyboard_handle_scancode() {
    uint8_t scancode = inb(PORT_KBD_DATA);
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
    uint8_t mask = inb(0x21);
    outb(0x21, mask & ~0x02);
}