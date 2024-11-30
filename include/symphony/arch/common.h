/**
 * @file arch/common.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * Common arch interface, subset of all the arch-specific interfaces.
 */

#pragma once

#include <symphony/types.h>

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

/**
 * @brief Allocate new, empty top-level page table.
 *
 * @return Pointer to the newly allocated top-level page table
 */
void* arch_vmm_new_pt(void);

/**
 * @brief Free top-level page table and all sub-page tables.
 *
 * @param pageTable Top-level page table to be destroyed.
 */
void arch_vmm_destroy_pt(void* pageTable);

/**
 * @brief Perform an address space switch on the current CPU.
 *
 * @param pageTable Top-level page table to be used for the switch.
 */
void arch_vmm_switch(void* pageTable);

/**
 * @brief Map a physical page to a virtual page.
 *
 * @param pageTable Top-level page table, the context of this operation
 * @param physAddr Physical address, preferably page-aligned
 * @param virtAddr Virtual address, preferably page-aligned
 * @param flags VMM flags 
 */
void arch_vmm_map(void* pageTable, uint64_t physAddr, uint64_t virtAddr, int flags);

/**
 * @brief Unmap a virtual page.
 *
 * @param pageTable Top-level page table, the context of this operation
 * @param virtAddr Virtual address, preferably page-aligned
 */
void arch_vmm_unmap(void* pageTable, uint64_t virtAddr);

/**
 * @brief Set virtual page flags.
 *
 * @param pageTable Top-level page table, the context of this operation
 * @param virtAddr Virtual address, preferably page-aligned
 * @param flags VMM flags
 */
void arch_vmm_set_flags(void* pageTable, uint64_t virtAddr, int flags);
