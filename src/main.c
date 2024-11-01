#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 16

int main(void) {
	unsigned char line[BUFFER_SIZE];
	size_t bytes_read;

	bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
	while (bytes_read == BUFFER_SIZE) {
		write(STDOUT_FILENO, line, BUFFER_SIZE);
		write(STDOUT_FILENO, "\n", 1);
		memset(line, 0, BUFFER_SIZE);
		bytes_read = read(STDIN_FILENO, line, BUFFER_SIZE);
	}
	write(STDOUT_FILENO, line, bytes_read);
	write(STDOUT_FILENO, "\n", 1);
}
