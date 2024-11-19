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

/**
 * @brief Initialize physical memory manager.
 *
 * @return 0 on success, negative error value on error.
 */
int pmm_init(void);
