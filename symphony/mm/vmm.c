/*
 * File: mm/vmm.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Virtual Memory Manager
 */

#include <symphony/mm.h>
#include <symphony/arch/arch.h>
#include <symphony/boot_proto.h>
#include <symphony/debug.h>

// Kernel top level page table.
void* kernelPT;

void vmm_map_range(void* pageTable, uint64_t physAddr, uint64_t virtAddr, size_t size, int flags) {
	if (size % PAGE_SIZE != 0) 
		(void)ALIGN_UP(size, PAGE_SIZE);

	for (size_t i = 0; i < size; i += PAGE_SIZE)
		vmm_map(pageTable, physAddr + i, virtAddr + i, flags);
}

void* vmm_kernel_pt(void) {
	assert(kernelPT != NULL, "Attempt to fetch root kernel page table before VMM initialization!\n");

	return kernelPT;
}

int vmm_init(void) {
	kernelPT = vmm_new_pt();

	struct boot_proto_memmap_entry entry;

	debug_log(LOGLEVEL_TRACE, "Mapping memmap entries...\n");

	for (uint64_t i = 0; i < boot_proto_memmap_entry_count(); i++) {
		entry = boot_proto_memmap_entry_get(i);

		if (entry.base == 0) {
			entry.base += PAGE_SIZE;
			if (entry.length >= PAGE_SIZE) 
				entry.length -= PAGE_SIZE;
			else
				entry.length = 0;
		}

		vmm_map_range(kernelPT, entry.base, entry.base + boot_proto_hhdm_offset(), entry.length, (VMM_PRESENT | VMM_RWX));	
	}

	debug_log(LOGLEVEL_TRACE, "Mapping kernel...\n");

	vmm_map_range(kernelPT, boot_proto_kernel_physical_base(), boot_proto_kernel_virtual_base(), boot_proto_kernel_size(), (VMM_PRESENT | VMM_RWX));

	vmm_switch(kernelPT);

	debug_log(LOGLEVEL_INFO, "VMM Initialized\n");

	return 0;
}
