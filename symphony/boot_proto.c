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
