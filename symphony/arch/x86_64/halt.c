// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: arch/x86_64/halt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * x86 halt function.
 */

#include <symphony/arch/x86_64.h>

void arch_halt(void) {
	asm volatile("cli");
	for (;;)
		asm volatile("hlt");
}
