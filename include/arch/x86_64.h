// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include <arch/common.h>
#include <types.h>

void arch_outb(uint32_t port, uint8_t data);
uint8_t arch_inb(uint32_t port);
void arch_outw(uint32_t port, uint16_t data);
uint16_t arch_inw(uint32_t port);
void arch_outl(uint32_t port, uint32_t data);
uint32_t arch_inl(uint32_t port);
