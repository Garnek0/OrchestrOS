/**
 * @file boot_proto.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 * 
 * @brief
 * Boot protocol abstraction layer.
 *
 * @details
 * Boot protocol abstraction layer. Most of the definitions in here coincide
 * with the stuff in "limine.h". This is because i did not want the kernel to
 * use those things directly, since it should not care about how the boot
 * protocol itself works (That is the job of this boot protocol abstraction
 * layer). This also means that if I ever decide to change the boot protocol
 * for whatever reason, it would simply be a matter of implementing all the
 * functions declared in here.
 */

#pragma once

#include <symphony/types.h>

/**@{*/
/** @brief Firmware type macro for boot_proto_firmware_type(). */
#define BOOT_PROTO_FW_UNKNOWN 0
#define BOOT_PROTO_FW_X86BIOS 1
#define BOOT_PROTO_FW_UEFI32 2
#define BOOT_PROTO_FW_UEFI64 3
/**@}*/

/**@{*/
/** @brief Memory map entry types. */
#define BOOT_PROTO_MEMMAP_UNKNOWN 0
#define BOOT_PROTO_MEMMAP_USABLE 1
#define BOOT_PROTO_MEMMAP_RESERVED 2
#define BOOT_PROTO_MEMMAP_ACPI_RECLAIMABLE 3
#define BOOT_PROTO_MEMMAP_ACPI_NVS 4
#define BOOT_PROTO_MEMMAP_BAD_MEMORY 5
#define BOOT_PROTO_MEMMAP_BOOTLOADER_RECLAIMABLE 6
#define BOOT_PROTO_MEMMAP_KERNEL_AND_MODULES 7
#define BOOT_PROTO_MEMMAP_FRAMEBUFFER 8
/**@}*/

/** @brief Memory map entry structure. */
struct boot_proto_memmap_entry {
	/** @brief Base physical address of this memory map entry */
	uint64_t base;

	/** @brief Length of this memory map entry in bytes */
	uint64_t length;

	/** @brief Memory map entry type. */
	uint64_t type;
};

/**
 * @brief Check if the current bootloader is supported.
 *
 * @return true if the bootloader is supported, false otherwise
 */
bool boot_proto_bl_supported(void);

/**
 * @brief Get the name of the current bootloader.
 *
 * @return ASCII string containing the name of the bootloader
 */
char* boot_proto_bl_name(void);

/**
 * @brief Get the version string of the current bootloader.
 *
 * @return ASCII string containing the version of the bootloader
 */
char* boot_proto_bl_version(void);

/**
 * @brief Get machine firmware type.
 *
 * @return Firmware type of the machine the kernel is running on. Firmware
 * type macros are defined in this file (boot_proto.h)
 */
uint64_t boot_proto_firmware_type(void);

/**
 * @brief Get Higher Half Direct Map (HHDM) offset.
 *
 * @return HHDM offset
 */
uint64_t boot_proto_hhdm_offset(void);

/**
 * @brief Get memory map entry count.
 *
 * @return The number of memory map entries
 */
uint64_t boot_proto_memmap_entry_count(void);

/**
 * @brief Get memory map entry.
 *
 * @param i Memory map entry index. If i is out of bounds, a kernel panic
 * will be triggered.
 *
 * @return boot_proto_memmap_entry structure containing the base address, 
 * length and type of the memory map entry
 */
struct boot_proto_memmap_entry boot_proto_memmap_entry_get(uint64_t i);

/**
 * @brief Get human-readable ASCII string from memory map entry type.
 *
 * @param type The memory map entry type as a 64-bit integer
 *
 * @return Human-readable ASCII string of the memory map entry type.
 */
const char* boot_proto_memmap_type_to_str(uint64_t type);

/**
 * @brief Get kernel physical base address.
 *
 * @return Kernel physical base address
 */
uint64_t boot_proto_kernel_physical_base(void);

/**
 * @brief Get kernel virtual base address.
 *
 * @return Kernel virtual base address
 */
uint64_t boot_proto_kernel_virtual_base(void);

/**
 * @brief Get kernel file size.
 *
 * @return Kernel file size
 */
uint64_t boot_proto_kernel_size(void);
