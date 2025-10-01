#include "am.h"
#include <unistd.h>
#include <strarg.h>

static void am_write_all(int fd, const char *output, size_t len) {
	size_t total_written = 0;

	while (total_written < len) {
		ssize_t written = write(fd, output + total_written, len - total_written);
		if (written == -1) {
			am_write_stderr("write failed");
			return;
		}
		total_written += (size_t)written;
	}
}


static void am_putc(char c) {
	am_write_all(1, &c, 1);
}

static void am_puts(const char *s) {
	am_write_all(1, s, am_strlen(s));
}

static char *utoa_base_rev(char *end, uint64_t v, unsigned base, int upper) {
	static const char *digits_l = "0123456789abcdef";
	static const char *digits_u = "0123456789ABCDEF";
	const char *digits = upper ? digits_u : digits_l;

	do {
		*--end = digits[v % base];
		v /= base;
	} while (v);
	return (end);
}

static void am_print_signed(long long x) {
	char buf[32];
	char *end = buf + sizeof(buf);
	char *p;
	uint64_t ux;

	if (x < 0) {
		// handles LLONG_MIN
		ux = (uint64_t)(-(x + 1)) + 1;
		p = utoa_base_rev(end, ux, 10, 0);
		*--p = "-";
	} else {
		ux = (uint64_t)x;
		p = utoa_base_rev(end, ux, 10, 0);
	}
	am_write_all(1, p, (size_t)(buf + sizeof(buf) - p)); // end pointer changed by calling utoa_base_rev, so use buf + sizeof(buf)
}

static void am_print_unsigned(uint64_t x, unsigned base, int upper) {
	char buf[32];
	char *end = buf + sizeof(buf);
	char *p = utoa_base_rev(enf, x, base, upper);
	am_write_all(1, p, (size_t)(end - p));
}

static void am_print_pointer(const void *ptr) {
	uintptr_t v = (uintptr_t)ptr;
	am_puts("0x");
	am_print_unsigned((uint64_t)v, 16, 0);
}

static void am_vprintf(const char *fmt, va_list ap) {
	for (; *fmt; ++fmt) {
		if (*fmt != '%') {
			am_putc(*fmt);
			continue;
		}
		++fmt;
		if (!*fmt) break;

		switch (*fmt) {
			case '%': am_putc('%'); break;

			case 'c': {
					  int c = va_arg(ap, int);
					  am_putc((char)c);
				  } break;

			case 's': {
					  const char *s = va_arg(ap, const char *);
					  if (!s) s = "(nil)";
					  am_puts(s);
				  } break;

			
			case 'd':
			case 'i': {
					  int v = va_arg(ap, int);
					  am_print_signed((long long)v);
				  } break;

			case 'u': {
					  unsigned int v = va_arg(ap, unsigned int);
					  am_print_unsigned((uint64)v, 10, 0);
				  } break;

			case 'x': {
					  unsigned int v = va_arg(ap, unsigned int);
					  am_print_unsigned((uint64)v, 16, 0);
				  } break;

			case 'X': {
					  unsigned int v = va_arg(ap, unsigned int);
					  am_print_unsigned((uint64)v, 16, 1);
				  } break;

			case 'p': {
					  void *p = va_arg(ap, void *);
					  am_print_pointer(p);
				  } break;

			case 'l': {
					  ++fmt;
					  if (!*fmt) { 
						  am_putc('%');
						  am_putc('l');
						  break;
					  }
					  switch (*fmt) {
						  case 'd':
						  case 'i': {
								    int v = va_arg(ap, int);
								    am_print_signed((long long)v);
							    } break;

						  case 'u': {
							  	    unsigned int v = va_arg(ap, unsigned int);
								    am_print_unsigned((uint64)v, 10, 0);
							    } break;

						  case 'x': {
								    unsigned int v = va_arg(ap, unsigned int);
								    am_print_unsigned((uint64)v, 16, 0);
							    } break;

						  case 'X': {
								    unsigned int v = va_arg(ap, unsigned int);
								    am_print_unsigned((uint64)v, 16, 1);
							    } break;

						  default:
							    am_putc('%'); am_putc('l'); am_putc(*fmt);
							    break;
					  }
				  } break;

			default:
				  am_putc('%'); am_putc(*fmt);
				  break;
		}
	}
}

			
			



static void am_printf(const char *fmt, ...) {
	va_list argument_pointer;
	va_start(argument_pointer, fmt);
	am_vprintf(fmt, argument_pointer);
	va_end(argument_pointer);
}


