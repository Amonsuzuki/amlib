#include "am.h"

#define POOL_SIZE 100

MemBlock memory_pool[POOL_SIZE];

MemBlock *am_malloc() {
	for (int i = 0; i < POOL_SIZE; i++) {
		if (!memory_pool[i].in_use) {
			memory_pool[i].in_use = 1;
			return (&memory_pool[i]);
		}
	}
	return (NULL);
}

void am_free(MemBlock *block) {
	block->in_use = 0;
}
