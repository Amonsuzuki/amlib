#include "am.h"
#include <stddef.h>
#include <stdint.h> 

size_t am_strlen(const char *str) {
        size_t len = 0;

        while (*str++) len++;
        return (len);
}
