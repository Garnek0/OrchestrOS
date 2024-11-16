// SPDX-License-Identifier: BSD-2-Clause

#include <arch/arch.h>

void arch_halt(void) {
	asm volatile("msr daifset, 0xf");
	for (;;) {
		asm volatile("wfi");
	}
}
