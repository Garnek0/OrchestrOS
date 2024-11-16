// SPDX-License-Identifier: BSD-2-Clause

#include <arch/x86_64.h>

void arch_halt(void) {
	asm volatile("cli");
	for (;;) {
		asm volatile("hlt");
	}
}
