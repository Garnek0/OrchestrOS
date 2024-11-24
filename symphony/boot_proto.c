/*
 * File: boot_proto.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Abstraction layer for the Limine boot protocol.
 */

#include <limine.h>
#include <symphony/boot_proto.h>
#include <symphony/debug.h>

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

// Set the Limine protocol base revision to 3.
__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// Bootloader Info Request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_bootloader_info_request bootloaderInfoRequest = {
    .id = LIMINE_BOOTLOADER_INFO_REQUEST,
    .revision = 0
};

// Firmware Type Request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_firmware_type_request firmwareTypeRequest = {
    .id = LIMINE_FIRMWARE_TYPE_REQUEST,
    .revision = 0
};

// HHDM Request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memoryMapRequest = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

// Memory Map Request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdmRequest = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

// Kernel Address Request
__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kernelAddressRequest = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

bool boot_proto_bl_supported(void) {
	// Ensure the bootloader actually understands our Limine base revision (see spec).
	if (!LIMINE_BASE_REVISION_SUPPORTED)
		return false;

	return true;
}

char* boot_proto_bl_name(void) {
	return bootloaderInfoRequest.response->name;
}

char* boot_proto_bl_version(void) {
	return bootloaderInfoRequest.response->version;
}

uint64_t boot_proto_firmware_type(void) {
	switch (firmwareTypeRequest.response->firmware_type) {
		case LIMINE_FIRMWARE_TYPE_X86BIOS:
			return BOOT_PROTO_FW_X86BIOS;
		case LIMINE_FIRMWARE_TYPE_UEFI32:
			return BOOT_PROTO_FW_UEFI32;
		case LIMINE_FIRMWARE_TYPE_UEFI64:
			return BOOT_PROTO_FW_UEFI64;
		default:
			return BOOT_PROTO_FW_UEFI64;
	}
}

uint64_t boot_proto_hhdm_offset(void) {
	return hhdmRequest.response->offset;
}

uint64_t boot_proto_memmap_entry_count(void) {
	return memoryMapRequest.response->entry_count;	
}

struct boot_proto_memmap_entry boot_proto_memmap_entry_get(uint64_t i) {
	struct boot_proto_memmap_entry entry = {0};

	assert(i < memoryMapRequest.response->entry_count, "index greater than the memmap entry count!\n");

	// Memory map entry provided by the bootloader
	struct limine_memmap_entry* blMemmapEntry = memoryMapRequest.response->entries[i];

	entry.base = blMemmapEntry->base;
	entry.length = blMemmapEntry->length;

	switch (blMemmapEntry->type) {
		case LIMINE_MEMMAP_USABLE:
			entry.type = BOOT_PROTO_MEMMAP_USABLE;
			break;
		case LIMINE_MEMMAP_RESERVED:
			entry.type = BOOT_PROTO_MEMMAP_RESERVED;
			break;
		case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
			entry.type = BOOT_PROTO_MEMMAP_ACPI_RECLAIMABLE;
			break;
		case LIMINE_MEMMAP_ACPI_NVS:
			entry.type = BOOT_PROTO_MEMMAP_ACPI_NVS;
			break;
		case LIMINE_MEMMAP_BAD_MEMORY:
			entry.type = BOOT_PROTO_MEMMAP_BAD_MEMORY;
			break;
		case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
			entry.type = BOOT_PROTO_MEMMAP_BOOTLOADER_RECLAIMABLE;
			break;
		case LIMINE_MEMMAP_KERNEL_AND_MODULES:
			entry.type = BOOT_PROTO_MEMMAP_KERNEL_AND_MODULES;
			break;
		case LIMINE_MEMMAP_FRAMEBUFFER:
			entry.type = BOOT_PROTO_MEMMAP_FRAMEBUFFER;
			break;
		default:
			entry.type = BOOT_PROTO_MEMMAP_UNKNOWN;
			break;
	}

	return entry;
}

const char* boot_proto_memmap_type_to_str(uint64_t type) {
	switch (type) {
		case BOOT_PROTO_MEMMAP_UNKNOWN:
			return "Unknown";
			break;
		case BOOT_PROTO_MEMMAP_USABLE:
			return "Usable";
			break;
		case BOOT_PROTO_MEMMAP_RESERVED:
			return "Reserved";
			break;
		case BOOT_PROTO_MEMMAP_ACPI_RECLAIMABLE:
			return "ACPI Reclaimable";
			break;
		case BOOT_PROTO_MEMMAP_ACPI_NVS:
			return "ACPI NVS";
			break;
		case BOOT_PROTO_MEMMAP_BAD_MEMORY:
			return "Bad Memory";
			break;
		case BOOT_PROTO_MEMMAP_BOOTLOADER_RECLAIMABLE:
			return "Bootloader Reclaimable";
			break;
		case BOOT_PROTO_MEMMAP_KERNEL_AND_MODULES:
			return "Kernel and Modules";
			break;
		case BOOT_PROTO_MEMMAP_FRAMEBUFFER:
			return "Framebuffer";
		default:
			return "Undefined Type";
	}
}

uint64_t boot_proto_kernel_physical_base(void) {
	return kernelAddressRequest.response->physical_base;
}

uint64_t boot_proto_kernel_virtual_base(void) {
	return kernelAddressRequest.response->virtual_base;
}
