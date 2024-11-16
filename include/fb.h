// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include <types.h>

int fb_init(void);
void fb_set_pixel(size_t x, size_t y, uint32_t color);
uint32_t fb_get_pixel(size_t x, size_t y);
