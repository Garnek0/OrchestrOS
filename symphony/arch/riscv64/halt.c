// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: arch/riscv64/halt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * riscv64 halt function.
 */

#include <symphony/arch/riscv64.h>

void arch_halt(void) {
	asm volatile("csrci mstatus, 0x8");
	asm volatile("csrci sstatus, 0x2");
	asm volatile("csrci ustatus, 0x1");
	for (;;)
		asm volatile("wfi");	
}
