#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 16

int main(void) {
	unsigned char line[BUFFER_SIZE + 1] = {0};
	size_t bytes_read;

	bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
	while (bytes_read > 0) {
		printf("%s\n", line);
		memset(line, 0, BUFFER_SIZE + 1);
		bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
	}
}
