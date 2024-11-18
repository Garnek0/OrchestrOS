/*
 * File: arch/aarch64/halt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * aarch64 halt function.
 */


#include <symphony/arch/aarch64.h>

void arch_halt(void) {
	asm volatile("msr daifset, 0xf");
	for (;;)
		asm volatile("wfi");
}
