#include "am.h"

int am_isdigit(unsigned char str) {
	if (47 < str && str < 58)
		return (1);
	return (0);
}
