#include "am.h"
#include <stddef.h>

void convert_to_num(const char *p, size_t *result, char **endptr) {
	int digit = 0;

	while (am_isdigit((unsigned char)*p)) {
		digit = *p - '0';
		if (*result > (SIZE_MAX - digit) / 10) {
			*result = SIZE_MAX;
			if (endptr)
				*endptr = (char *)p;
			return;
		}
		*result = *result * 10 + digit;
		p++;
	}
	if (endptr)
		*endptr = (char *)p;
}

size_t am_strtol(const char *str, char **endptr) {
	const char *p = str;
	size_t result = 0;
	int sign = 1;

	while (am_isspace((unsigned char)*p))
		p++;
	if (*p == '-') {
		sign = -1;
		p++;
	}
	else if (*p == '+')
		p++;
	convert_to_num(p, &result, endptr);
	return (sign * result);
}
