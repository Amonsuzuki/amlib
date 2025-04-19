#include "am.h"
#include <unistd.h>

void am_print(const char *output) {
	size_t len = am_strlen(output);
	size_t total_written = 0;
	ssize_t written;

	while (total_written < len) {
		written = write(1, output + total_written, len - total_written);
		if (written == -1) {
			am_write_stderr("write failed");
			return;
		}
		total_written += written;
	}
}
