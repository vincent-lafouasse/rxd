#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 16

bool is_printable(char c) {
    // below are control characters, above (0x7f) is DEL
    return (c >= 0x20 && c <= 0x7e);
}

// displays non printable characters as dots
void display(const char* input) {
    char line[BUFFER_SIZE + 1] = {0};
    strncpy(line, input, BUFFER_SIZE);

    for (size_t i = 0; line[i]; i++) {
        if (!is_printable(line[i])) {
            line[i] = '.';
        }
    }

    write(STDOUT_FILENO, line, strlen(line));
    write(STDOUT_FILENO, "\n", 1);
}

int main(void) {
    char line[BUFFER_SIZE + 1] = {0};
    size_t bytes_read;

    bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
    while (bytes_read > 0) {
        display(line);
        memset(line, 0, BUFFER_SIZE + 1);
        bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
    }
}
