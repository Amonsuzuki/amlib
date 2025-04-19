#include "am.h"

int am_isspace(unsigned char str) {
	if (str == ' ' || str == '	')
		return (1);
	return (0);
}
