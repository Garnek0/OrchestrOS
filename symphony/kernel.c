/*
 * File: kernel.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Kernel entry point and initialization stuff.
 */

#include <symphony/kernel.h>
#include <symphony/debug.h>
#include <symphony/serial.h>
#include <symphony/types.h>
#include <limine.h>
#include <symphony/arch/arch.h>

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
		debug_log(LOGLEVEL_FATAL, "Kernel Limine protocol base revision not supported by the bootloader!\n");
		arch_halt();
	}

	debug_printf("Init done.\n");

	arch_halt();
}
