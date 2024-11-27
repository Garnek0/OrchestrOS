/*
 * File: debug.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
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

#define LENGTH_NONE 0
#define LENGTH_INT 1
#define LENGTH_LONG 2
#define LENGTH_LONG_LONG 3
#define LENGTH_SIZE 4
#define LENGTH_INTMAX 5
#define LENGTH_PTRDIFF 6
#define LENGTH_PTR 7

#define MAX_WIDTH 256

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

// Internal integer formatting function for debug_vprintf().
static int debug_print_int(uintmax_t n, int flags, int width, int base, bool sign, bool upper) {
	int chars = 0;
	bool negative = (((intmax_t)n < 0) && sign) ? true : false;

	const char digitsLower[] = "0123456789abcdef";
	const char digitsUpper[] = "0123456789ABCDEF";

	uintmax_t revn = 0;	
	int dcount = 0;

	if (negative)
		n = (uintmax_t)-((intmax_t)n);

	if (base > 16)
		return 0;

	do {
		dcount++;
		revn *= base;
		revn += n%base;
		n /= base;
	} while (n != 0);

	if (negative) {
		debug_putchar('-');
		chars++;
	} else {
		if (flags & PLUS_FOR_POSITIVE) {
			debug_putchar('+');
			chars++;
		} else if (flags & SPACE_FOR_POSITIVE) {
			debug_putchar(' ');
			chars++;
		}
	}

	if (base == 8 && (flags & SPECIAL_HASH)) {
		debug_putchar('0');
	} else if (base == 16 && (flags & SPECIAL_HASH)) {
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

	for (int i = 0; i < dcount; i++) {
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
	int length;

	uintmax_t value;

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

		if (width > MAX_WIDTH)
			width = MAX_WIDTH;

		length = 0;

		// Get the length
		switch (fmt[i]) {
			case 'h':
				if(fmt[i+1] == 'h')
					i++;
				length = LENGTH_INT;
				i++;
				break;
			case 'l':
				if(fmt[i+1] == 'l') {
					length = LENGTH_LONG_LONG;
					i++;
				} else {
					length = LENGTH_LONG;
				}
				i++;
				break;
			case 'z':
				length = LENGTH_SIZE;
				i++;
				break;
			case 'j':
				length = LENGTH_INTMAX;
				i++;
				break;
			case 't':
				length = LENGTH_PTRDIFF;
				i++;
				break;
			default:
				if (fmt[i] == 'p' || fmt[i] == 's')
					length = LENGTH_PTR;
				else 
					length = LENGTH_INT;
				break;
		}

		value = 0;

		// Get the value
		switch (fmt[i]) {
			case 'd':
			case 'i':
				if (length == LENGTH_INT)
					value = (uintmax_t)((int)va_arg(args, int));
				else if (length == LENGTH_LONG)
					value = (uintmax_t)((long)va_arg(args, long));
				else if (length == LENGTH_LONG_LONG)
					value = (uintmax_t)((long long)va_arg(args, long long));
				else if (length == LENGTH_SIZE)
					value = (uintmax_t)((size_t)va_arg(args, size_t));
				else if (length == LENGTH_INTMAX)
					value = (uintmax_t)((intmax_t)va_arg(args, intmax_t));
				else if (length == LENGTH_PTRDIFF)
					value = (uintmax_t)((ptrdiff_t)va_arg(args, ptrdiff_t));
				break;
			case 'u':
			case 'x':
			case 'X':
			case 'o':
			case 'c':
				if (length == LENGTH_INT)
					value = (uintmax_t)((unsigned int)va_arg(args, unsigned int));
				else if (length == LENGTH_LONG)
					value = (uintmax_t)((unsigned long)va_arg(args, unsigned long));
				else if (length == LENGTH_LONG_LONG)
					value = (uintmax_t)((unsigned long long)va_arg(args, unsigned long long));
				else if (length == LENGTH_SIZE)
					value = (uintmax_t)((size_t)va_arg(args, size_t));
				else if (length == LENGTH_INTMAX)
					value = (uintmax_t)((uintmax_t)va_arg(args, uintmax_t));
				else if (length == LENGTH_PTRDIFF)
					value = (uintmax_t)((ptrdiff_t)va_arg(args, ptrdiff_t));
				break;
			case 'p':
				value = (uintmax_t)((void*)va_arg(args, void*));
				break;
			case 's':
				value = (uintmax_t)((const char*)va_arg(args, const char*));
				break;
			default:
				break;
		}

		// Do the printing
		switch (fmt[i]) {
			case '%':
				chars += debug_putchar('%');
				break;
			case 'd':
			case 'i':
				chars += debug_print_int((intmax_t)value, flags, width, 10, true, false);
				break;
			case 'u':
				chars += debug_print_int(value, flags, width, 10, false, false);
				break;
			case 'x':
				chars += debug_print_int(value, flags, width, 16, false, false);
				break;
			case 'X':
				chars += debug_print_int(value, flags, width, 16, false, true);
				break;
			case 'p':
				chars += debug_print_int(value, flags, width, 16, false, true);
				break;
			case 'o':
				chars += debug_print_int(value, flags, width, 8, false, false);
				break;
			case 's':
				if(!value)
					chars += debug_print("(null)");
				else
					chars += debug_print((const char*)value);
				break;
			case 'c':
				chars += 1;
				debug_putchar((char)value);
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

void debug_panic(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	debug_log(LOGLEVEL_FATAL, "Kernel Panic! ");
	debug_vprintf(fmt, args);

	va_end(args);

	arch_halt();

	__builtin_unreachable();
}

void __debug_assert(int cond, const char* message, const char* func, const char* file, int line) {
	if (!cond) {
		debug_log(LOGLEVEL_FATAL, "Assertion failed in %s (%s:%d): %s", func, file, line, message);
		arch_halt();
	}
}

void __debug_assert_warn(int cond, const char* message, const char* func, const char* file, int line) {
	if (!cond)
		debug_log(LOGLEVEL_WARN, "Assertion failed in %s (%s:%d): %s", func, file, line, message);
}
