/*
 * File: arch/riscv64/init.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * riscv64 initialization.
 */

#include <symphony/arch/arch.h>

int arch_init_very_early(int cpu) {
	(void)cpu;
	return 0;
}

int arch_init_early(int cpu) {
	(void)cpu;
	return 0;
}

int arch_init_late(int cpu) {
	(void)cpu;
	return 0;
}
