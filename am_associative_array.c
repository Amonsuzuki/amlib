#include "am.h"

uint32_t hash32(int key) {
	uint32_t h = (uint32_t)key;

	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return (h);
}

uint64_t hash64(uint64_t x) {
	x ^= x >> 30;
	x *= 0xbf58476d1ce4e5b9ULL;
	x ^= x >> 27;
	x *= 0x94d049bb133111ebULL;
	x ^= x >> 31;
	return x;
}


void insert_ii(ii *array, long long int key, long long int value, long long int box_size) {
	uint64_t hashed_index;
	int distance;
	uint64_t new_index;

	hashed_index = hash64(key) % box_size;
	if (!array[hashed_index].is_filled) {
		array[hashed_index].key = key;
		array[hashed_index].value = value;
		array[hashed_index].distance = 0;
		array[hashed_index].is_filled = 1;
	}
	else {
		distance = 1;
		new_index = (hashed_index + distance) % box_size;
		while (array[new_index].is_filled) {
			if (array[new_index].distance < distance) {
				int tmp_key = array[new_index].key;
				int tmp_value = array[new_index].value;
				int tmp_distance = array[new_index].distance;
				array[new_index].key = key;
				array[new_index].value = value;
				array[new_index].distance = distance;
				key = tmp_key;
				value = tmp_value;
				distance = tmp_distance;
			}
			new_index = (new_index + 1) % box_size;
			distance++;
		}
		array[new_index].key = key;
		array[new_index].value = value;
		array[new_index].distance = distance;
		array[new_index].is_filled = 1;
	}
}

//allow collision for unordered_set
//ABC193 C
void insert_ii_unordered(ii *array, long long int key, long long int value, long long int box_size) {
	uint64_t hashed_index;
	int distance;
	uint64_t new_index;

	hashed_index = hash64(key) % box_size;
	if (!array[hashed_index].is_filled) {
		array[hashed_index].key = key;
		array[hashed_index].value = value;
		array[hashed_index].distance = 0;
		array[hashed_index].is_filled = 1;
	}
	else {
		distance = 1;
		new_index = (hashed_index + distance) % box_size;
		while (array[new_index].is_filled) {
			if (array[new_index].key == key)
				return;
			else {
				if (array[new_index].distance < distance) {
					int tmp_key = array[new_index].key;
					int tmp_value = array[new_index].value;
					int tmp_distance = array[new_index].distance;
					array[new_index].key = key;
					array[new_index].value = value;
					array[new_index].distance = distance;
					key = tmp_key;
					value = tmp_value;
					distance = tmp_distance;
				}
				new_index = (new_index + 1) % box_size;
				distance++;
			}
		}
		array[new_index].key = key;
		array[new_index].value = value;
		array[new_index].distance = distance;
		array[new_index].is_filled = 1;
	}
}

int search_ii(ii *array, int key, int box_size) {
	uint32_t hashed_index;

	hashed_index = hash32(key) % box_size;
	while (array[hashed_index].is_filled) {
		if (array[hashed_index].key == key)
			return array[hashed_index].value;
		hashed_index++;
	}
	//not sufficient error handliing
	return (-1);
}

int delete_ii(ii *array, int key, int box_size) {
	uint32_t hashed_index;

	hashed_index = hash32(key) % box_size;
	while (array[hashed_index].is_filled) {
		if (array[hashed_index].key == key) {
			array[hashed_index].is_filled = 0;
			return (array[hashed_index].value);
		}
		hashed_index++;
	}
	return (-1);
}
