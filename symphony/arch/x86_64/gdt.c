/*
 * File: arch/x86_64/gdt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * x86 Global Descriptor Table.
 */

#include <symphony/arch/arch.h>

struct gdtr {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

struct gdt_entry {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t access;
	uint8_t limit1 : 4;
	uint8_t flags : 4;
	uint8_t base2;
} __attribute__((packed));

struct gdt_system_entry {
	uint16_t limit0;
	uint16_t base0;
	uint8_t base1;
	uint8_t access;
	uint8_t limit1 : 4;
	uint8_t flags : 4;
	uint8_t base2;
	uint32_t base3;
	uint32_t reserved;
} __attribute__((packed));

struct gdt {
	struct gdt_entry null;
	struct gdt_entry kernelCode;
	struct gdt_entry kernelData;
	struct gdt_entry userCode;
	struct gdt_entry userData;
	struct gdt_system_entry tss;
} __attribute__((packed));

struct tss {
	uint32_t reserved0;
	uint64_t rsp[3];
	uint64_t reserved1;
	uint64_t ist[7];
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t iomapBase;
} __attribute__((packed));

struct gdt gdt[1];
struct tss tss[1];
struct gdtr gdtr[1];

// Set access and flags of a GDT entry. Limit and base are ignored on
// 64-bit x86 and are therefore set to 0.
static void gdt_set_entry(struct gdt_entry* entry, uint8_t access, uint8_t flags) {
	entry->access = access;
	entry->flags = flags;
	entry->base0 = entry->base1 = entry->base2 = 0;
	entry->limit0 = entry->limit1 = 0;
}

// Same as the above function, except this one works with system entries which
// have a different structure and the base and limit fields need to have
// meaningful values.
static void gdt_set_system_entry(struct gdt_system_entry* entry, uint8_t access,
								 uint8_t flags, uint64_t base, uint32_t limit) {
	entry->access = access;
	entry->flags = flags;
	entry->base0 = (base & 0xFFFF);
	entry->base1 = (base >> 16) & 0xFF;
	entry->base2 = (base >> 24) & 0xFF;
	entry->base3 = (base >> 32) & 0xFFFFFFFF;
	entry->limit0 = (limit & 0xFFFF);
	entry->limit1 = (limit >> 16) & 0xF;
}

void arch_load_gdt(int cpu) {
	gdt_set_entry(&gdt[cpu].null, 0, 0);
	gdt_set_entry(&gdt[cpu].kernelCode, 0x9A, 0xA);
	gdt_set_entry(&gdt[cpu].kernelData, 0x92, 0xC);
	gdt_set_entry(&gdt[cpu].userCode, 0xFA, 0xA);
	gdt_set_entry(&gdt[cpu].userData, 0xF2, 0xC);
	gdt_set_system_entry(&gdt[cpu].tss, 0xE9, 0x0, (uint64_t)&tss[cpu], sizeof(struct tss));

	gdtr[cpu].offset = (uint64_t)&gdt[cpu];
	gdtr[cpu].size = sizeof(struct gdt) - 1;

	// Load the GDT
	asm volatile (
		"lgdt (%0)\n"
		"movw $0x10, %%ax\n"
		"movw %%ax, %%ds\n"
		"movw %%ax, %%es\n"
		"movw %%ax, %%fs\n"
		"movw %%ax, %%gs\n"
		"movw %%ax, %%ss\n"
		"popq %%rax\n"
		"pushq $0x08\n"
		"pushq %%rax\n"
		"retfq\n"
		:: "r"(&gdtr[cpu]) : "memory", "rax");

	// Load the TSS
	asm volatile ("movw $0x28, %ax; ltr %ax;");
}

void arch_set_kernel_stack(int cpu, void* stack) {
	tss[cpu].rsp[0] = (uint64_t)stack;	
}
