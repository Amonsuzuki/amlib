#include "am.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int read_header(int fd) {
	char magic[6];

	am_read(fd, magic, 6);
	if (magic[0] != '\x93' || magic[1] != 'N' || magic[2] != 'U' || magic[3] != 'M' || magic[4] != 'P' || magic[5] != 'Y') {
		am_printf("Not a valid .npy format\n");
		close(fd);
		exit(1);
		return (-1);
	}
	return (0);
}

int read_version(int fd) {
	unsigned char version[2];

	am_read(fd, version, 2);
	return (0);
}

void get_shape(char *header, size_t **shape, size_t *ndim) {
	const char *shape_start = am_strstr(header, "'shape': (");

	if (!shape_start) {
		am_printf("Error: 'shape' not found\n");
		return;
	}
	shape_start += 10;
	*ndim = 1;
	for (const char *p = shape_start; *p && *p != ')'; p++) {
		if (*p == ',')
			(*ndim)++;
	}
	*shape = (size_t *)malloc(*ndim * sizeof(size_t));
	if (!*shape) {
		am_printf("Memory allocation failed\n");
		exit(1);
	}
	for (size_t i = 0; i < *ndim; i++) {
		(*shape)[i] = am_strtol(shape_start, (char **)&shape_start);
		while (*shape_start && (*shape_start == ',' || *shape_start == ' '))
			shape_start++;
	}
}

char *read_shape(int fd, size_t **shape, size_t *ndim) {
	unsigned short header_len;

	am_read(fd, &header_len, 2);
	char *header = (char *)malloc(header_len + 1);
	if (!header) {
		am_printf("Memory allocation failed\n");
		exit(1);
	}
	am_read(fd, header, header_len);
	header[header_len] = '\0';
	get_shape(header, shape, ndim);
	/*
	am_printf("Shape: ");
	for (size_t i = 0; i < *ndim; i++) {
		printff("%zu", (*shape)[i]);
		if (i < *ndim - 1)
			printff(", ");
	}
	printf("\n");
	*/
	free(header);
	return (NULL);
}

Array am_read_npy(const char *filename) {
	int fd = open(filename, O_RDONLY);
	size_t *shape = NULL;
	size_t ndim = 0;
	size_t total_elements = 1;

	if (fd == -1) {
		am_printf("Error opening file");
		exit(1);
	}
	read_header(fd);
	read_version(fd);
	read_shape(fd, &shape, &ndim);
	for (size_t i = 0; i < ndim; i++)
		total_elements *= shape[i];
	double *data = (double *)malloc(total_elements * sizeof(double));
	am_read(fd, data, total_elements * sizeof(double));
	close(fd);
	Array result = {
		.data = data,
		.shape = shape,
		.ndim = ndim,
		.total_elements = total_elements
	};
	return (result);
}
