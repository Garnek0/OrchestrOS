// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: debug.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * Various debug functions.
 */

#include <symphony/debug.h>
#include <symphony/string.h>
#include <symphony/arch/arch.h>
#include <symphony/serial.h>

#define LEFT_ALIGN 1
#define PLUS_FOR_POSITIVE (1 << 1)
#define SPACE_FOR_POSITIVE (1 << 2)
#define PREPEND_ZEROES (1 << 3)
#define THOUSANDS_GROUPING (1 << 4)
#define SPECIAL_HASH (1 << 5)

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9') 

char debug_putchar(char chr) {
#ifdef __x86_64__
	// 0xE9 Port hack for x86 QEMU/Bochs.
	arch_outb(0xE9, chr);
#endif
	serial_char(chr);

	return chr;
}

int debug_print(const char* str) {
	for (size_t i = 0; i < strlen(str); i++)
		debug_putchar(str[i]);

	return strlen(str);
}

static int debug_print_int(uintmax_t n, int flags, int width, int size, int base, bool sign, bool upper) {
	int chars = 0;
	bool negative = (((intmax_t)n < 0) && sign) ? true : false;

	const char digitsLower[] = "0123456789abcdef";
	const char digitsUpper[] = "0123456789ABCDEF";

	if (negative)
		n = (uintmax_t)-((intmax_t)n);

	if (base > 16)
		return 0;

	uintmax_t revn = 0;
	int dcount = 0;

	do {
		dcount++;
		revn *= base;
		revn += n%base;
		n /= base;
	} while (n != 0);

	if (negative) {
		debug_putchar('-');
		chars++;
		width--;
	} else {
		if (flags & PLUS_FOR_POSITIVE) {
			debug_putchar('+');
			chars++;
			width--;
		} else if (flags & SPACE_FOR_POSITIVE) {
			debug_putchar(' ');
			chars++;
			width--;
		}
	}

	if (base == 8 && (flags & SPECIAL_HASH)) {
		width -= 1;	
		debug_putchar('0');
	} else if (base == 16 && (flags & SPECIAL_HASH)) {
		width -= 2;
		debug_putchar('0');
		if (upper)
			debug_putchar('X');
		else
			debug_putchar('x');
	}

	if ((width - dcount) > 0 && !(flags & LEFT_ALIGN)) {
		if (flags & PREPEND_ZEROES) {
			for (int i = 0; i < width - dcount; i++) {
				debug_putchar('0');
				chars++;
			}
		} else {
			for (int i = 0; i < (width - dcount); i++) {
				debug_putchar(' ');
				chars++;
			}
		}
	}

	for (int i = 0; i < dcount; i++){
		if (upper)
			debug_putchar(digitsUpper[revn % base]);
		else 
			debug_putchar(digitsLower[revn % base]);

		revn /= base;
		chars++;
	}

	if ((width - dcount) > 0 && (flags & LEFT_ALIGN)) {
		if (flags & PREPEND_ZEROES) {
			for (int i = 0; i < width - dcount; i++) {
				debug_putchar('0');
				chars++;
			}
		} else {
			for (int i = 0; i < width - dcount; i++) {
				debug_putchar(' ');
				chars++;
			}
		}
	}

	(void)size; // size unused, for now at least

	return chars;
}

int debug_printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int chars;

	chars = debug_vprintf(fmt, args);
	va_end(args);

	return chars;
}

int debug_vprintf(const char* fmt, va_list args) {
	int chars = 0;

	int flags, prevflags;
	int width;
	int size;

	for (size_t i = 0; i < strlen(fmt); i++) {
		if (fmt[i] != '%') {
			debug_putchar(fmt[i]);
			chars++;
			continue;
		}

		flags = prevflags = 0;

		do {
			i++;
			prevflags = flags;
			switch (fmt[i]) {
				case '-':
					flags |= LEFT_ALIGN;
					break;
				case '+':
					flags |= PLUS_FOR_POSITIVE;
					break;
				case ' ':
					flags |= SPACE_FOR_POSITIVE;
					break;
				case '0':
					flags |= PREPEND_ZEROES;
					break;
				case '\'':
					flags |= THOUSANDS_GROUPING;
					break;
				case '#':
					flags |= SPECIAL_HASH;
					break;
				default:
					break;
			}
		} while (flags != prevflags);

		width = 0;

		while (IS_DIGIT(fmt[i])) {
			width *= 10;
			width += (int)(fmt[i] - '0');

			i++;
		}

		size = 0;

		switch (fmt[i]) {
			case 'h':
				size = sizeof(int);
				if(fmt[i+1] == 'h')
					i++;
				break;
			case 'l':
				if(fmt[i+1] == 'l') {
					size = sizeof(long long);
					i++;
				} else {
					size = sizeof(long);
				}
				break;
			case 'z':
				size = sizeof(size_t);
				break;
			case 'j':
				size = sizeof(intmax_t);
				break;
			case 't':
				size = sizeof(ptrdiff_t);
				break;
			default:
				break;
		}

		if (size)
			i++;
		else 
			size = sizeof(int);

		switch (fmt[i]) {
			case '%':
				chars += debug_putchar('%');
				break;
			case 'd':
			case 'i':
				chars += debug_print_int((uintmax_t)va_arg(args, int), flags, width, size, 10, true, false);
				break;
			case 'u':
				chars += debug_print_int((uintmax_t)va_arg(args, int), flags, width, size, 10, false, false);
				break;
			case 'x':
				chars += debug_print_int((uintmax_t)va_arg(args, int), flags, width, size, 16, false, false);
				break;
			case 'p':
			case 'X':
				chars += debug_print_int((uintmax_t)va_arg(args, int), flags, width, size, 16, false, true);
				break;
			case 'o':
				chars += debug_print_int((uintmax_t)va_arg(args, int), flags, width, size, 8, false, false);
				break;
			case 's':
				chars += debug_print((const char*)va_arg(args, const char*));
				break;
			case 'c':
				chars += 1;
				debug_putchar((char)va_arg(args, int));
				break;
			case 'n':
				*va_arg(args, int*) = chars;
				break;
			default:	
				break;
		}
	}

	return chars;
}

int debug_log(int loglevel, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int chars;

	switch (loglevel) {
		case LOGLEVEL_TRACE:
			debug_print("[ TRACE ] ");
			break;
		case LOGLEVEL_DEBUG:
			debug_print("[ DEBUG ] ");
			break;
		case LOGLEVEL_INFO:
			debug_print("[ INFO  ] ");
			break;
		case LOGLEVEL_WARN:
			debug_print("[ WARN  ] ");
			break;
		case LOGLEVEL_ERROR:
			debug_print("[ ERROR ] ");
			break;
		case LOGLEVEL_FATAL:
			debug_print("[ FATAL ] ");
			break;
		default:
			debug_print("[ UNK ] ");
			break;
	}

	chars = debug_vprintf(fmt, args);
	va_end(args);

	return chars;
}

void __debug_assert(int cond, const char* message, const char* func, const char* file, int line) {
	if (!cond) {
		debug_log(LOGLEVEL_FATAL, "Assertion failed in %s (%s:%d): %s", func, file, line, message);
		arch_halt();
	}
}

void __debug_assert_warn(int cond, const char* message, const char* func, const char* file, int line) {
	if(!cond)
		debug_log(LOGLEVEL_WARN, "Assertion failed in %s (%s:%d): %s", func, file, line, message);
}
