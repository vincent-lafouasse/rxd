#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KWHT  "\x1B[37m"

enum Color {
    WHITE,
    BLUE,
    GREEN,
    RED,
    YELLOW,
};

int fprintf_colored(enum Color color, FILE* f, const char *format, ...)
{
    switch (color) {
        case WHITE:
            fprintf(f, KWHT);
            break;
        case BLUE:
            fprintf(f, KBLU);
            break;
        case GREEN:
            fprintf(f, KGRN);
            break;
        case RED:
            fprintf(f, KRED);
            break;
        case YELLOW:
            fprintf(f, KYEL);
            break;
        default:
            fprintf(f, KNRM);
    }

    va_list args;
    va_start(args, format);
    int status = fprintf(f, format, args);
    va_end(args);

    fprintf(f, KNRM);

    return status;
}

/*
0x00 = white
0xff = blue
printable = green
non-printable = red
tabs and linebreaks = yellow
*/

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct {
    size_t line_width;  // 0 means one line
    FILE* in;
    FILE* out;
} DisplayConfig;

DisplayConfig default_config(void);
void putstr(const char* s, int fd);
void mem_reverse(void* __bytes, size_t n);
bool is_printable(char c);

void display_bytes_hex(const char* input, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);

    for (size_t i = 0; input[i] && i < cfg.line_width; i++) {
        fprintf(cfg.out, "%02x", (u32)input[i]);
        if (i % 2 == 1) {
            fprintf(cfg.out, " ");
        }
    }
}

// displays non printable characters as dots
void display_bytes_ascii(const char* line, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);

    for (size_t i = 0; line[i] && i < cfg.line_width; i++) {
        if (is_printable(line[i]))
            fprintf_colored(GREEN, cfg.out, "%c", line[i]);
        else
            fprintf_colored(YELLOW, cfg.out, ".");
    }
}

int main(void) {
    const DisplayConfig cfg = default_config();
    char* line = calloc(cfg.line_width + 1, 1);
    u32 offset = 0;
    size_t bytes_read;

    bytes_read = fread(line, cfg.line_width, 1, cfg.in);
    while (bytes_read > 0) {
        fprintf(cfg.out, "%08u: ", offset);
        offset += bytes_read;
        display_bytes_hex(line, cfg);
        fprintf(cfg.out, " ");
        display_bytes_ascii(line, cfg);
        fprintf(cfg.out, "\n");
        memset(line, 0, cfg.line_width + 1);
        bytes_read = fread(line, cfg.line_width, 1, cfg.in);
    }
    free(line);
}

DisplayConfig default_config(void) {
    return (DisplayConfig){.line_width = 16, .in = stdin, .out = stdout};
}

bool is_printable(char c) {
    // below are control characters, above (0x7f) is DEL
    return (c >= 0x20 && c <= 0x7e);
}
