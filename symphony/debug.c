// SPDX-License-Identifier: BSD-2-Clause

#include <debug.h>
#include <string.h>
#include <arch/arch.h>

//NOTE: This is a stub.
int debug_print(const char* str) {
	for (size_t i = 0; i < strlen(str); i++) {
#ifdef __x86_64__
	arch_outb(0xE9, str[i]);
#endif
	}

	return strlen(str);
}
