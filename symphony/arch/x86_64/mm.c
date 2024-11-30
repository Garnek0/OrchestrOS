/*
 * File: arch/x86_64/mm.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * x86 memory management implementation.
 */

#include <symphony/arch/arch.h>
#include <symphony/mm.h>
#include <symphony/boot_proto.h>
#include <symphony/string.h>

// Get all page table indexes given a virtual address.
static void page_index(uint64_t virtAddr, int* Pi, int* PTi, int* PDi, int* PDPi) {
	virtAddr >>= 12;
	*Pi = virtAddr & 0x1ff;
	virtAddr >>= 9;
	*PTi = virtAddr & 0x1ff;
	virtAddr >>= 9;
	*PDi = virtAddr & 0x1ff;
	virtAddr >>= 9;
	*PDPi = virtAddr & 0x1ff;
}

// Get the pointer to a page table entry using page table indexes. This function
// also allocates any missing page tables.
static struct page_table_entry* page_from_index(struct page_table* pt, int Pi, int PTi, int PDi, int PDPi) {
	struct page_table_entry currentEntry;

	currentEntry = pt->entries[PDPi];
    struct page_table* PDP;
    if (!currentEntry.present) {
        PDP = (struct page_table*)((uint64_t)pmm_alloc(1) + boot_proto_hhdm_offset());
        memset((void*)PDP, 0, sizeof(struct page_table));

        currentEntry.addr = (uint64_t)((uint64_t)PDP - boot_proto_hhdm_offset()) >> 12;
        currentEntry.present = true;
        currentEntry.readWrite = true;
        currentEntry.userSupervisor = true;
        pt->entries[PDPi] = currentEntry;
    } else {
        PDP = (struct page_table*)(((uint64_t)currentEntry.addr << 12) + boot_proto_hhdm_offset());
    }

    currentEntry = PDP->entries[PDi];
    struct page_table* PD;
    if (!currentEntry.present) {
        PD = (struct page_table*)((uint64_t)pmm_alloc(1) + boot_proto_hhdm_offset());
        memset((void*)PD, 0, sizeof(struct page_table));

        currentEntry.addr = (uint64_t)((uint64_t)PD - boot_proto_hhdm_offset()) >> 12;
        currentEntry.present = true;
        currentEntry.readWrite = true;
        currentEntry.userSupervisor = true;
        PDP->entries[PDi] = currentEntry;
    } else {
        PD = (struct page_table*)(((uint64_t)currentEntry.addr << 12) + boot_proto_hhdm_offset());
    }

    currentEntry = PD->entries[PTi];
    struct page_table* PT;
    if (!currentEntry.present) {
        PT = (struct page_table*)((uint64_t)pmm_alloc(1) + boot_proto_hhdm_offset());
        memset((void*)PT, 0, sizeof(struct page_table));

        currentEntry.addr = (uint64_t)((uint64_t)PT - boot_proto_hhdm_offset()) >> 12;
        currentEntry.present = true;
        currentEntry.readWrite = true;
        currentEntry.userSupervisor = true;
        PD->entries[PTi] = currentEntry;
    } else {
        PT = (struct page_table*)(((uint64_t)currentEntry.addr << 12) + boot_proto_hhdm_offset());
    }

	return &PT->entries[Pi];
}

void* arch_vmm_new(void) {
	struct page_table* pt = (struct page_table*)((uint64_t)pmm_alloc(1) + boot_proto_hhdm_offset());
	memset((void*)pt, 0, sizeof(struct page_table));

	return (void*)pt;
}

void arch_vmm_switch(void* pageTable) {
	struct page_table* pt = (struct page_table*)((uint64_t)pageTable - boot_proto_hhdm_offset());
	asm volatile("mov %0, %%cr3" : : "r" (pt));
}

void arch_vmm_map(void* pageTable, uint64_t physAddr, uint64_t virtAddr, int flags) {
	struct page_table* pt = (struct page_table*)pageTable;

	int Pi, PTi, PDi, PDPi;
    page_index(virtAddr, &Pi, &PTi, &PDi, &PDPi);

	struct page_table_entry* pte = page_from_index(pt, Pi, PTi, PDi, PDPi); 

    pte->addr = (uint64_t)physAddr >> 12;
	arch_vmm_set_flags(pt, virtAddr, flags);
}

void arch_vmm_unmap(void* pageTable, uint64_t virtAddr) {
	struct page_table* pt = (struct page_table*)pageTable;

	int Pi, PTi, PDi, PDPi;
    page_index(virtAddr, &Pi, &PTi, &PDi, &PDPi);

	struct page_table_entry* pte = page_from_index(pt, Pi, PTi, PDi, PDPi);

	pte->present = 0;
    pte->addr = 0;

    asm("invlpg (%0)" :: "r"(virtAddr) : "memory");
}

void arch_vmm_set_flags(void* pageTable, uint64_t virtAddr, int flags) {
	struct page_table* pt = (struct page_table*)pageTable;

	int Pi, PTi, PDi, PDPi;
    page_index(virtAddr, &Pi, &PTi, &PDi, &PDPi);

	struct page_table_entry* pte = page_from_index(pt, Pi, PTi, PDi, PDPi);

	pte->present = (flags & VMM_PRESENT);
    pte->readWrite = (flags & VMM_RW);
    pte->userSupervisor = (flags & VMM_USER);
    pte->pageSize = false;
    pte->nx = !(flags & VMM_EXEC);
}
