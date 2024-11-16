// SPDX-License-Identifier: BSD-2-Clause

#include <debug.h>
#include <types.h>
#include <fb.h>
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
	debug_print("Symphony is starting...\n");

    // Ensure the bootloader actually understands our Limine base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
		debug_print("Limine base revision not supported by the bootloader!\n");
        arch_halt();
    }

	fb_init();	

    for (size_t i = 0; i < 100; i++) {
        for (size_t j = 0; j < 100; j++) {
			fb_set_pixel(i, j, 0xFFFFFFFF);
		}
    }

    // We're done, just hang...
    arch_halt();
}
