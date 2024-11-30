/**
 * @file mm.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * Memory management interface.
 */

#pragma once

#include <symphony/types.h>
#include <symphony/string.h>
#include <symphony/arch/arch.h>

/**
 * @brief Present page flag.
 */
#define VMM_PRESENT 1

/**
 * @brief Read/write page.
 */
#define VMM_RW (1 << 1)

/**
 * @brief Execute page.
 */
#define VMM_EXEC (1 << 2)

/**
 * @brief Read/write/execute page.
 */
#define VMM_RWX (VMM_RW | VMM_EXEC)

/**
 * @brief User-accesible page.
 */
#define VMM_USER (1 << 3)

/**
 * @brief Get the closest value greater than x aligned on the specified byte boundary.
 *
 * @param x The value to calculate the alignment from
 * @param alignment The alignment to use for the calculation
 */
#define ALIGN_UP(x, alignment) (((uint64_t)x % alignment == 0) ? (typeof(x))(x) : (typeof(x))((uint64_t)x + (alignment - ((uint64_t)x % alignment))))

/**
 * @brief Standard page size.
 */
#define PAGE_SIZE 4096

/**
 * @brief Initialize the physical memory manager.
 *
 * @return 0 on success, negative error value on error.
 */
int pmm_init(void);

/**
 * @brief Allocate physical memory pages.
 *
 * @param pages Number of PAGE_SIZE pages to allocate.
 *
 * @return Non-HHDM base address of the newly allocated memory
 */
void* pmm_alloc(int pages);

/**
 * @brief Deallocate physical memory pages.
 *
 * @param base Non-HHDM base address to start deallocating from.
 * @param pages Number of pages to deallocate.
 *
 * @warning This function does not check whether the memory to be deallocated 
 * has actually been previously allocated or not. This means that some 
 * very-hard-to-track-down oopsies may be introduced in the kernel if it is 
 * fed the wrong parameters. The Symphony PMM tries to make it easier to spot 
 * accidentally deallocated memory by setting all deallocated bytes to 0xFF 
 * (a common strategy).
 *
 * @return 0 on success, negative error value on error.
 */
int pmm_free(void* base, int pages);

/**
 * @brief Initialize the virtual memory manager.
 *
 * @return 0 on success, negative error value on error.
 */
int vmm_init(void);

/**
 * @brief Alias of arch_vmm_new().
 */
inline void* vmm_new(void) {
	return arch_vmm_new();
}

/**
 * @brief Alias of arch_vmm_switch().
 */
inline void vmm_switch(void* pageTable) {
	arch_vmm_switch(pageTable);
}

/**
 * @brief Alias of arch_vmm_map().
 */
inline void vmm_map(void* pageTable, uint64_t physAddr, uint64_t virtAddr, int flags) {
	arch_vmm_map(pageTable, physAddr, virtAddr, flags);
}

/**
 * @brief Alias of arch_vmm_unmap().
 */
inline void vmm_unmap(void* pageTable, uint64_t virtAddr) {
	arch_vmm_unmap(pageTable, virtAddr);
}

/**
 * @brief Alias of arch_vmm_set_flags().
 */
inline void vmm_set_flags(void* pageTable, uint64_t virtAddr, int flags) {
	arch_vmm_set_flags(pageTable, virtAddr, flags);
}

/**
 * @brief Map a physical address range to a virtual address range.
 *
 * @param pageTable Top-level page table, the context of this operation
 * @param physAddr Physical address, preferably page-aligned
 * @param virtAddr Virtual address, preferably page-aligned
 * @param size Range size in bytes
 * @param flags VMM flags 
 */
void vmm_map_range(void* pageTable, uint64_t physAddr, uint64_t virtAddr, size_t size, int flags);
