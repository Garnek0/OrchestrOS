// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: kernel.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * Kernel entry point and initialization stuff.
 */

#include <kernel.h>
#include <debug.h>
#include <serial.h>
#include <types.h>
#include <limine.h>
#include <arch/arch.h>

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

// Set the Limine protocol base revision to 3.
__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Kernel entry point
void _start(void) {
	serial_init();
	debug_printf("Symphony "KERNEL_VER_STRING" is starting...\n");

	// Ensure the bootloader actually understands our Limine base revision (see spec).
	if (LIMINE_BASE_REVISION_SUPPORTED == false) {
		debug_print("Kernel Limine protocol base revision not supported by the bootloader!\n");
		arch_halt();
	}	

	debug_printf("Init done.\n");

	arch_halt();
}
