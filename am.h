#ifndef AM_H
#define AM_H

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
	double *data;
	size_t *shape;
	size_t ndim;
	size_t total_elements;
} Array;

typedef struct {
	int in_use;
	double data[10];
} MemBlock;

typedef struct {
	long long int key;
	long long int value;
	int distance;
	int is_filled;
}ii;

size_t am_strlen(const char *str);
void am_print(const char *str);
Array am_read_npy(const char *filename);
int am_isdigit(unsigned char str);
int am_isspace(unsigned char str);
char *am_strstr(const char *haystack, const char *needle);
size_t am_strtol(const char *str, char **endptr);
void am_write_stderr(const char *msg);
ssize_t am_read(int fd, void *buf, size_t size);
MemBlock *am_malloc();
void am_free(MemBlock *block);
uint32_t hash32(int key);
uint64_t hash64(uint64_t key);
void insert_ii(ii *array, long long int key, long long int value, long long int box_size);
void insert_ii_unordered(ii *array, long long int key, long long int value, long long int box_size);
int search_ii(ii *array, int key, int box_size);
int delete_ii(ii *array, int key, int box_size);

#endif
