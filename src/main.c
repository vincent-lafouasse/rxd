#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef uint8_t u8;
typedef uint32_t u32;

typedef struct {
    size_t line_width;  // 0 means one line
    int fd_in;
    int fd_out;
} DisplayConfig;

DisplayConfig default_config(void);
void putstr(const char* s, int fd);
void mem_reverse(void* __bytes, size_t n);
bool is_printable(char c);

// displays non printable characters as dots
void display_ascii_with_dots(const char* input, const DisplayConfig cfg) {
    assert(cfg.line_width != 0);
    char* line = calloc(cfg.line_width + 1, 1);
    strncpy(line, input, cfg.line_width);

    for (size_t i = 0; line[i]; i++) {
        if (!is_printable(line[i])) {
            line[i] = '.';
        }
    }

    putstr(line, cfg.fd_out);
    free(line);
}

void display_hex(u32 n, const DisplayConfig cfg) {
    size_t width = sizeof(n);

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
    putstr(buffer, cfg.fd_out);
    free(buffer);
}

int main(void) {
    const DisplayConfig cfg = default_config();
    char* line = calloc(cfg.line_width + 1, 1);
    size_t bytes_read;
    u32 offset = 0;

    bytes_read = read(cfg.fd_in, line, cfg.line_width);
    while (bytes_read > 0) {
        display_hex(offset, cfg);
        putstr(": ", cfg.fd_out);
        offset += bytes_read;
        display_ascii_with_dots(line, cfg);
        putstr("\n", cfg.fd_out);
        memset(line, 0, cfg.line_width + 1);
        bytes_read = read(cfg.fd_in, line, cfg.line_width);
    }
    free(line);
}

DisplayConfig default_config(void) {
    return (DisplayConfig){
        .line_width = 16, .fd_in = STDIN_FILENO, .fd_out = STDOUT_FILENO};
}

void putstr(const char* s, int fd) {
    write(fd, s, strlen(s));
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
