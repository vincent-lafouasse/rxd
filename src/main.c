#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        char to_print = is_printable(line[i]) ? line[i] : '.';
        printf("%c", to_print);
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
