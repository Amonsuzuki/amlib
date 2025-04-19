#include "am.h"
#include <unistd.h>

ssize_t am_read(int fd, void *buf, size_t size) {
	size_t total_read = 0;
	ssize_t bytes_read;

	while (total_read < size) {
		bytes_read = read(fd, (char *)buf + total_read, size - total_read);
		if (bytes_read < 0) {
			am_print("Error: read failed\n");
			return (-1);
		}
		if (bytes_read == 0) {
			am_print("Waring: unexpected EOF\n");
			break;
		}
		total_read += bytes_read;
	}
	return (total_read);
}
