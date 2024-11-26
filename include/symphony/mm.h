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
 * @brief Initialize physical memory manager.
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
