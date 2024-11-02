#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KWHT "\x1B[37m"

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct {
    size_t line_width;  // 0 means one line
    FILE* in;
    FILE* out;
} DisplayConfig;

DisplayConfig default_config(void);

/*
0x00 = white
0xff = blue
printable = green
non-printable = red
tabs and linebreaks = yellow
*/
void set_correct_color(u8 c, DisplayConfig cfg);
bool is_printable(u8 c);

void display_bytes_hex(const u8* input, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);

    for (size_t i = 0; i < cfg.line_width; i++) {
        u8 c = input[i];
        set_correct_color(c, cfg);
        fprintf(cfg.out, "%02x", (u32)c);
        fprintf(cfg.out, KNRM);
        if (i % 2 == 1) {
            fprintf(cfg.out, " ");
        }
    }
}

// displays non printable characters as dots
void display_bytes_ascii(const u8* line, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);

    for (size_t i = 0; i < cfg.line_width; i++) {
        set_correct_color(line[i], cfg);
        if (is_printable(line[i]))
            fprintf(cfg.out, "%c" KNRM, line[i]);
        else
            fprintf(cfg.out, "." KNRM);
    }
}

int main(void) {
    const DisplayConfig cfg = default_config();
    u8* line = calloc(cfg.line_width + 1, 1);
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

void set_correct_color(u8 c, DisplayConfig cfg) {
    if (c == 0x00)
        fprintf(cfg.out, KWHT);
    else if (c == 0xff)
        fprintf(cfg.out, KBLU);
    else if (isspace(c))
        fprintf(cfg.out, KYEL);
    else if (is_printable(c))
        fprintf(cfg.out, KGRN);
    else
        fprintf(cfg.out, KRED);
}

bool is_printable(u8 c) {
    // below are control characters, above (0x7f) is DEL
    return (c >= 0x20 && c <= 0x7e);
}
