/*
 * File: arch/x86_64/ports.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * x86 port I/O helpers.
 */

#include <symphony/arch/arch.h>

void arch_outb(uint32_t port, uint8_t data) {
	asm volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

uint8_t arch_inb(uint32_t port) {
	uint8_t data;
	asm volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
	return data;
}

void arch_outw(uint32_t port, uint16_t data) {
	asm volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

uint16_t arch_inw(uint32_t port) {
	uint16_t data;
	asm volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
	return data;
}

void arch_outl(uint32_t port, uint32_t data) {
	asm volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

uint32_t arch_inl(uint32_t port) {
	uint32_t data;
	asm volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
	return data;
}
