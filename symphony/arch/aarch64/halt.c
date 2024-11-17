// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: arch/aarch64/halt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * aarch64 halt function.
 */


#include <arch/aarch64.h>

void arch_halt(void) {
	asm volatile("msr daifset, 0xf");
	for (;;)
		asm volatile("wfi");
}
