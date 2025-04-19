#include "am.h"
#include <unistd.h>
#include <errno.h>

ssize_t am_write(int fd, const char *buffer, size_t size) {
	size_t total_written = 0;
	ssize_t written;

	while (total_written < size) {
		written = write(fd, buffer + total_written, size - total_written);
		if (written < 0)
			return (-1);
		total_written += written;
	}
	return (total_written);
}

void am_write_stderr(const char *msg) {
        ssize_t written = write(2, msg, am_strlen(msg));

        if (written == -1) {
                const char *err = "Error writinf to stderr\n";
                am_write(2, err, am_strlen(err));
        }
        else if ((size_t)written < am_strlen(msg)) {
                const char *warn = "Partial write to stderr\n";
                am_write(2, warn, am_strlen(warn));
        }
}
