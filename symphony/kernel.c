/*
 * File: kernel.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Kernel entry point and initialization stuff.
 */

#include <symphony/kernel.h>
#include <symphony/debug.h>
#include <symphony/serial.h>
#include <symphony/types.h>
#include <symphony/arch/arch.h>
#include <symphony/mm.h>
#include <symphony/boot_proto.h>

// Kernel entry point
void _start(void) {
	if (arch_init_very_early(0) != 0)
		arch_halt();

	serial_init();

	debug_printf("Symphony "KERNEL_VER_STRING" is starting...\n");	

	if (!boot_proto_bl_supported())
		debug_panic("Bootloader not supported!\n");

	debug_log(LOGLEVEL_INFO, "Fetching Limine-compliant bootloader info...\n");
	debug_log(LOGLEVEL_INFO, "Bootloader name: %s\n", boot_proto_bl_name());
	debug_log(LOGLEVEL_INFO, "Bootloader version: %s\n", boot_proto_bl_version());
	debug_log(LOGLEVEL_INFO, "Finished fetching bootloader info.\n");

	debug_log(LOGLEVEL_INFO, "System firmware: ");

	switch (boot_proto_firmware_type()) {
		case BOOT_PROTO_FW_X86BIOS:
			debug_print("x86 BIOS\n");
			break;
		case BOOT_PROTO_FW_UEFI32:
			debug_print("32-bit UEFI\n");
			break;
		case BOOT_PROTO_FW_UEFI64:
			debug_print("64-bit UEFI\n");
			break;
		default:
			debug_print("Unknown firmware\n");
			break;
	}

	debug_log(LOGLEVEL_INFO, "HHDM offset: %#llx\n", boot_proto_hhdm_offset());
	debug_log(LOGLEVEL_INFO, "Kernel physical base: %#llx\n", boot_proto_kernel_physical_base());
	debug_log(LOGLEVEL_INFO, "Kernel virtual base: %#llx\n", boot_proto_kernel_virtual_base());

	struct boot_proto_memmap_entry entry;
	for (uint64_t i = 0; i < boot_proto_memmap_entry_count(); i++) {
		entry = boot_proto_memmap_entry_get(i);
		
		debug_log(LOGLEVEL_INFO, "memmap entry: %#llx->%#llx %s\n", entry.base, entry.base+entry.length, 
			   boot_proto_memmap_type_to_str(entry.type));
	}

	if (pmm_init() != 0)
		debug_panic("PMM initialization failed!\n");

	if (vmm_init() != 0)
		debug_panic("VMM initialization failed!\n");

	if(kheap_init() != 0)
		debug_panic("Kernel heap initialization failed\n");

	debug_log(LOGLEVEL_INFO, "Init done\n");

	arch_halt();
}
