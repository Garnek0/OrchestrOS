/**
 * @file arch/common.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * Common arch interface, subset of all the arch-specific interfaces.
 */

#pragma once

/**
 * @brief Halt the CPU.
 */
void arch_halt(void);

/**
 * @brief Initialize current processor (very early stage)
 *
 * @note The arch-specific implementation of this function must not use any
 * kernel features whatsoever. This function is the first thing that gets
 * called when the kernel is loaded, and therefore none of the kernel
 * subsystems are initialized.
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_very_early(void);

/**
 * @brief Initialize current processor (early stage).
 *
 * @note The arch-specific implementation of this function must assume
 * nothing besides basic debug logging and the physical memory manager is
 * set up when called and therefore it should use only the features of the 
 * aformentioned components.
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_early(void);

/**
 * @brief Initialize current processor (late stage).
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_late(void);

/**
 * @brief Initialize current processor (very early, early and late stages, in
 * this order).
 *
 * @return 0 on success, negative error value on error
 */
inline int arch_init_full(void) {
	int status;

	status = arch_init_very_early();

	if (status != 0)
		return status;

	status = arch_init_early();

	if (status != 0)
		return status;

	return arch_init_late();
}
