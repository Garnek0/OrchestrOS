// SPDX-License-Identifier: BSD-2-Clause
/**
 * @file arch/x86_64.h
 * @author Popa Vlad (Garnek0)
 *
 * @brief
 * x86_64 arch-specific interface.
 */

#pragma once

#include <symphony/arch/common.h>
#include <symphony/types.h>

/**
 * @brief Write byte value to x86 I/O port.
 *
 * @param port Port
 * @param data Byte value to be written
 */
void arch_outb(uint32_t port, uint8_t data);

/**
 * @brief Read byte value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Byte value read from port
 */
uint8_t arch_inb(uint32_t port);

/**
 * @brief Write word value to x86 I/O port.
 *
 * @param port Port
 * @param data Word value to be written
 */
void arch_outw(uint32_t port, uint16_t data);

/**
 * @brief Read word value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Word value read from port
 */
uint16_t arch_inw(uint32_t port);

/**
 * @brief Write long value to x86 I/O port.
 *
 * @param port Port
 * @param data Long value to be written
 */
void arch_outl(uint32_t port, uint32_t data);

/**
 * @brief Read long value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Long value read from port
 */
uint32_t arch_inl(uint32_t port);

