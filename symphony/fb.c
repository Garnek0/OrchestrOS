// SPDX-License-Identifier: BSD-2-Clause

// Generic framebuffer driver

#include "limine.h"
#include <fb.h>

static bool framebufferExists = false;

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

int fb_init(void) {
	if (framebuffer_request.response == NULL || 
	framebuffer_request.response->framebuffer_count == 0) {
		return 0;
	}

	framebufferExists = true;

	return 0;
}

// NOTE: we assume the framebuffer model is RGB with 32-bit pixels.

void fb_set_pixel(size_t x, size_t y, uint32_t color) {
	if(!framebufferExists) return;

	// Fetch the first framebuffer.
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

	uint64_t fbIndex = (fb->pitch/(fb->bpp/8))*y + x;
	uint32_t* fbAddr = (uint32_t*)fb->address;

	fbAddr[fbIndex] = color;
}

uint32_t fb_get_pixel(size_t x, size_t y) {
	if(!framebufferExists) return 0;

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

	uint64_t fbIndex = (fb->pitch/(fb->bpp/8))*y + x;
	uint32_t* fbAddr = (uint32_t*)fb->address;

	return fbAddr[fbIndex];
}
