/*
 * File: arch/x86_64/halt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
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
