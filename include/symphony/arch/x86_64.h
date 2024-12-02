/**
 * @file arch/x86_64.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * x86_64 arch-specific interface.
 */

#pragma once

#include <symphony/arch/common.h>

/** @brief x86 page table entry. */
struct page_table_entry {
	/**@{*/
	/** @brief Page table entry field documented in the Intel SDM */
	bool present : 1;
	bool readWrite : 1;
	bool userSupervisor : 1;
	bool writeThrough : 1;
	bool cacheDisable : 1;
	bool accessed : 1;
	bool avl0 : 1;
	bool pageSize : 1;
	uint8_t avl1 : 4;
	uint64_t addr : 51;
	bool nx : 1;
	/**@}*/
} __attribute__((packed));

/** @brief x86 page table. Consists of 512 entries. */
struct page_table {
	/** @brief 512 page table entries */
	struct page_table_entry entries[512];
} __attribute__((packed)) __attribute__((aligned(0x1000)));

struct regs {
	uint64_t ds;
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;

	// For interrupts
	uint64_t intn, errCode;

	uint64_t rip, cs, rflags, rsp, ss;
};

/**
 * @brief Write byte value to x86 I/O port.
 *
 * @param port Port
 * @param data Byte value to write
 */
void arch_outb(uint32_t port, uint8_t data);

/**
 * @brief Read byte value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Byte value read from port
 */
uint8_t arch_inb(uint32_t port);

/**
 * @brief Write word value to x86 I/O port.
 *
 * @param port Port
 * @param data Word value to write
 */
void arch_outw(uint32_t port, uint16_t data);

/**
 * @brief Read word value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Word value read from port
 */
uint16_t arch_inw(uint32_t port);

/**
 * @brief Write long value to x86 I/O port.
 *
 * @param port Port
 * @param data Long value to write
 */
void arch_outl(uint32_t port, uint32_t data);

/**
 * @brief Read long value from x86 I/O port.
 *
 * @param port Port
 *
 * @return Long value read from port
 */
uint32_t arch_inl(uint32_t port);

/**
 * @brief Load a Symphony-compatible Global Descriptor Table.
 *
 * @param cpu The CPU calling this function.
 */
void arch_load_gdt(int cpu);

/**
 * @brief Set an entry in the IDT.
 *
 * @param entry Entry index
 * @param isr Pointer to the Interrupt Service Routine
 * @param pdplGateType IDT entry flags
 */
void arch_idt_set_entry(uint8_t entry, void* isr, uint8_t pdplGateType);

/**
 * @brief Initialise interrupts.
 *
 * @return 0 on success, negative error value on error
 */
int arch_interrupt_init(void);
