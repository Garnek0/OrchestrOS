// SPDX-License-Identifier: BSD-2-Clause

#include <types.h>
#include <limine.h>
#include <arch/arch.h>

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

// Set the Limine protocol base revision to 3.
__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

// Kernel entry point
void _start(void) {
    // Ensure the bootloader actually understands our Limine base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        arch_halt();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || 
		framebuffer_request.response->framebuffer_count < 1) {
        arch_halt();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    // NOTE: we assume the framebuffer model is RGB with 32-bit pixels.
    for (size_t i = 0; i < 100; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }

    // We're done, just hang...
    arch_halt();
}
