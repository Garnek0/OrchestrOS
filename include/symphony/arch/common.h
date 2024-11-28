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
 * @param cpu The CPU calling this function
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_very_early(int cpu);

/**
 * @brief Initialize current processor (early stage).
 *
 * @note The arch-specific implementation of this function must assume
 * nothing besides basic debug logging and the physical memory manager is
 * set up when called and therefore it should use only the features of the 
 * aformentioned components.
 *
 * @param cpu The CPU calling this function
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_early(int cpu);

/**
 * @brief Initialize current processor (late stage).
 *
 * @param cpu The CPU calling this function
 *
 * @return 0 on success, negative error value on error
 */
int arch_init_late(int cpu);

/**
 * @brief Initialize current processor (very early, early and late stages, in
 * this order).
 *
 * @param cpu The CPU calling this function
 *
 * @return 0 on success, negative error value on error
 */
inline int arch_init_full(int cpu) {
	int status;

	status = arch_init_very_early(cpu);

	if (status != 0)
		return status;

	status = arch_init_early(cpu);

	if (status != 0)
		return status;

	return arch_init_late(cpu);
}

/**
 * @brief Set the kernel stack that should be used by a specific CPU when entering
 * kernel space.
 *
 * @param cpu The CPU to set the kernel stack for
 * @param stack Kernel stack pointer
 */
void arch_set_kernel_stack(int cpu, void* stack);
