#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LOWERCASE_HEX "0123456789abcdef"
#define UPPERCASE_HEX "0123456789abcdef"

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

    char* buffer = malloc(3 * cfg.line_width +
                          1);  // each bytes needs 2 hex digits and a space
    buffer[3 * cfg.line_width] = '\0';
    for (size_t i = 0; i < cfg.line_width; i++) {
        u8 byte = input[i];
        buffer[3 * i] = LOWERCASE_HEX[byte / 16];
        buffer[3 * i + 1] = LOWERCASE_HEX[byte % 16];
        buffer[3 * i + 2] = ' ';
    }
    fprintf(cfg.out, "%s", buffer);
    free(buffer);
}

// displays non printable characters as dots
void display_bytes_ascii(const char* input, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);
    char* line = calloc(cfg.line_width + 1, 1);
    strncpy(line, input, cfg.line_width);

    for (size_t i = 0; line[i]; i++) {
        if (!is_printable(line[i])) {
            line[i] = '.';
        }
    }

    fprintf(cfg.out, "%s", line);
    free(line);
}

void display_u32_hex(u32 n, const DisplayConfig cfg) {
    size_t width = 2 * sizeof(n);  // 2 hex per byte

    char* buffer = malloc(width + 1);
    memset(buffer, '0', width);
    buffer[width] = '\0';

    for (size_t i = 0; i < width; i++) {
        size_t digit = n % 16;
        if (digit < 10)
            buffer[i] = '0' + digit;
        else
            buffer[i] = (digit - 10) + 'A';
        n = n / 16;
    }
    mem_reverse(buffer, width);
    fprintf(cfg.out, "%s", buffer);
    free(buffer);
}

int main(void) {
    const DisplayConfig cfg = default_config();
    char* line = calloc(cfg.line_width + 1, 1);
    u32 offset = 0;
    size_t bytes_read;

    bytes_read = fread(line, cfg.line_width, 1, cfg.in);
    while (bytes_read > 0) {
        display_u32_hex(offset, cfg);
        fprintf(cfg.out, ": ");
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

void mem_reverse(void* __bytes, size_t n) {
    u8* bytes = __bytes;

    for (size_t i = 0; i < n / 2; i++) {
        u8 tmp = bytes[i];
        bytes[i] = bytes[n - 1 - i];
        bytes[n - 1 - i] = tmp;
    }
}
