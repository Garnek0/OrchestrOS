/**
 * @file boot_proto.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 * 
 * @brief
 * Limine boot protocol abstraction layer.
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
