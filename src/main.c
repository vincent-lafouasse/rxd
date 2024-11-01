#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 16

void putstr(const char* s, int fd) {
    write(fd, s, strlen(s));
}

bool is_printable(char c) {
    // below are control characters, above (0x7f) is DEL
    return (c >= 0x20 && c <= 0x7e);
}

// displays non printable characters as dots
void display_ascii_with_dots(const char* input, int fd) {
    char line[BUFFER_SIZE + 1] = {0};
    strncpy(line, input, BUFFER_SIZE);

    for (size_t i = 0; line[i]; i++) {
        if (!is_printable(line[i])) {
            line[i] = '.';
        }
    }

    putstr(line, fd);
}

// sprintf maybe
void display_hex(size_t offset, size_t n_digits);

int main(void) {
    char line[BUFFER_SIZE + 1] = {0};
    size_t bytes_read;
    size_t offset = 0;

    bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
    while (bytes_read > 0) {
        printf("%04zx: ", offset);
        fflush(stdout);
        offset += bytes_read;
        display_ascii_with_dots(line, STDOUT_FILENO);
        putstr("\n", STDOUT_FILENO);
        memset(line, 0, BUFFER_SIZE + 1);
        bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
    }
}
