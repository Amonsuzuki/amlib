#include "am.h"
#include <unistd.h>
#include <stdarg.h>
#include <math.h>

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
		*--p = '-';
	} else {
		ux = (uint64_t)x;
		p = utoa_base_rev(end, ux, 10, 0);
	}
	am_write_all(1, p, (size_t)(buf + sizeof(buf) - p)); // end pointer changed by calling utoa_base_rev, so use buf + sizeof(buf)
}

static void am_print_unsigned(uint64_t x, unsigned base, int upper) {
	char buf[32];
	char *end = buf + sizeof(buf);
	char *p = utoa_base_rev(end, x, base, upper);
	am_write_all(1, p, (size_t)(end - p));
}

static void am_print_pointer(const void *ptr) {
	uintptr_t v = (uintptr_t)ptr;
	am_puts("0x");
	am_print_unsigned((uint64_t)v, 16, 0);
}

static uint64_t am_pow10_u(unsigned n) {
	static const uint64_t p10[] = {
		1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL,
		1000000ULL, 10000000ULL, 100000000ULL, 1000000000ULL, 10000000000ULL, 100000000000ULL,
		1000000000000ULL, 10000000000000ULL, 100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL, 1000000000000000000ULL
	};
	if (n < sizeof(p10)/sizeof(p10[0])) return p10[n];
	return 1000000000000000000ULL;
}

static void am_print_float(double x, unsigned precision) {
	if (precision > 18) precision = 18;

	if (isnan(x)) { am_puts("nan"); return; }
	if (isinf(x)) { am_puts((x < 0) ? "-inf" : "inf"); return; }

	if (x < 0.0) {
		am_putc('-');
		x = -x;
	}

	uint64_t ipart = (uint64_t)x;
	double frac = x - (double)ipart;

	uint64_t scale = am_pow10_u(precision);
	double scaled = frac * (double)scale + 0.5;
	uint64_t fpart = (uint64_t)scaled;

	if (fpart >= scale) {
		ipart += 1;
		fpart = 0;
	}

	am_print_unsigned(ipart, 10, 0);

	if (precision == 0) return;

	am_putc('.');

	char buf[24];
	char *end = buf + sizeof(buf);
	char *p = end;

	if (fpart == 0) {
		for (unsigned i = 0; i < precision; ++i) am_putc('0');
		return;
	}

	uint64_t temp = fpart;
	while (temp) {
		*--p = (char)('0' + (temp % 10));
		temp /= 10;
	}

	unsigned ndig = (unsigned)(end - p);
	if (ndig < precision)
		for (unsigned i = 0; i < precision - ndig; ++i) am_putc('0');
	am_write(1, p, (size_t)(end - p));
}



static void am_vprintf(const char *fmt, va_list ap) {
	for (; *fmt; ++fmt) {
		if (*fmt != '%') {
			am_putc(*fmt);
			continue;
		}
		++fmt;
		if (!*fmt) break;

		unsigned precision = 6;
		if (*fmt == '.') {
			++fmt;
			precision = 0;
			while ('0' <= *fmt && *fmt <= '9') {
				precision = precision * 10 + (unsigned)(*fmt - '0');
				++fmt;
			}
			if (!*fmt) break;
		}

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
					  am_print_unsigned((uint64_t)v, 10, 0);
				  } break;

			case 'x': {
					  unsigned int v = va_arg(ap, unsigned int);
					  am_print_unsigned((uint64_t)v, 16, 0);
				  } break;

			case 'X': {
					  unsigned int v = va_arg(ap, unsigned int);
					  am_print_unsigned((uint64_t)v, 16, 1);
				  } break;

			case 'p': {
					  void *p = va_arg(ap, void *);
					  am_print_pointer(p);
				  } break;

			case 'f': {
					  double d = va_arg(ap, double);
					  am_print_float(d, precision);
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
								    am_print_unsigned((uint64_t)v, 10, 0);
							    } break;

						  case 'x': {
								    unsigned int v = va_arg(ap, unsigned int);
								    am_print_unsigned((uint64_t)v, 16, 0);
							    } break;

						  case 'X': {
								    unsigned int v = va_arg(ap, unsigned int);
								    am_print_unsigned((uint64_t)v, 16, 1);
							    } break;

						  case 'f': {
								    double d = va_arg(ap, double);
								    am_print_float(d, precision);
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

			
			

void am_printf(const char *fmt, ...) {
	va_list argument_pointer;
	va_start(argument_pointer, fmt);
	am_vprintf(fmt, argument_pointer);
	va_end(argument_pointer);
}

