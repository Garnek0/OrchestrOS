/*
 * File: arch/x86_64/interrupt.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * x86 interrupts.
 */

#include <symphony/arch/arch.h>
#include <symphony/debug.h>

#define INT_GATE 0x8E
#define INT_USER_GATE 0xEE
#define TRAP_GATE 0x8F

struct idtr {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

struct idt_entry {
	uint16_t offset0;
	uint16_t seg;
	uint8_t ist;
	uint8_t pdplGateType;
	uint16_t offset1;
	uint32_t offset2;
	uint32_t reserved;
} __attribute__((packed));

// Exception ISRs. Implemented in arch/x86_64/interrupt.asm

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

static struct idtr idtr;

// There are 256 entries in an x86 IDT.
static struct idt_entry idt[256];

static const char* exceptionMessages[] = {
	"Divide Error",
	"Debug",
	"NMI Triggered",
	"Breakpoint Reached",
	"Overflow",
	"Bound Range Exceeded",
	"Invalid Opcode",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack Segment Fault",
	"General Protection Fault",
	"Page Fault",
	"(Reserved)",
	"x87 Floating-Point Fault",
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Fault",
	"Virtualisation Fault",
	"Control Protection Fault",
	"(Reserved)",
	"(Reserved)",
	"(Reserved)",
	"(Reserved)",
	"(Reserved)",
	"(Reserved)",
	"Hypervisor Injection",
	"VMM Communication",
	"Security Fault",
	"(Reserved)"
};

void arch_exception_handler(struct regs* regs) {
	uint64_t cr0, cr2, cr3, cr4;

	asm volatile(
		"push %%rax\n"
		"mov %%cr0, %%rax\n"
		"mov %%rax, %0\n"
		"mov %%cr2, %%rax\n"
		"mov %%rax, %1\n"
		"mov %%cr3, %%rax\n"
		"mov %%rax, %2\n"
		"mov %%cr4, %%rax\n"
		"mov %%rax, %3\n"
		"pop %%rax\n"
	: "=r" (cr0), "=r" (cr2), "=r" (cr3), "=r" (cr4) :: "%rax");

	debug_panic("%s!\n"
			 "RAX = %#016llx RBX = %#016llx RCX = %#016llx\n"
			 "RDX = %#016llx RSI = %#016llx RDI = %#016llx\n"
			 "RBP = %#016llx RSP = %#016llx RIP = %#016llx\n"
			 "R8  = %#016llx R9  = %#016llx R10 = %#016llx\n"
			 "R11 = %#016llx R12 = %#016llx R13 = %#016llx\n"
			 "R14 = %#016llx R15 = %#016llx\n"
			 "RFLAGS = %#016llx\n"
			 "CS = %#llx DS = %#llx SS = %#llx\n"
			 "CR0 = %#016llx CR2 = %#016llx\n"
			 "CR3 = %#016llx CR4 = %#016llx\n"
			 "V = %#016llx ERR = %#016llx",
			 exceptionMessages[regs->intn], regs->rax, regs->rbx, regs->rcx,
			 regs->rdx, regs->rsi, regs->rdi,
			 regs->rbp, regs->rsp, regs->rip,
			 regs->r8, regs->r9, regs->r10,
			 regs->r11, regs->r12, regs->r13,
			 regs->r14, regs->r15,
			 regs->rflags,
			 regs->cs, regs->ds, regs->ss,
			 cr0, cr2, 
			 cr3, cr4,
			 regs->intn, regs->errCode);
}

void arch_idt_set_entry(uint8_t entry, void* isr, uint8_t pdplGateType) {
	idt[entry].offset0 = (uint64_t)isr & 0xFFFF;
	idt[entry].offset1 = ((uint64_t)isr >> 16) & 0xFFFF;
	idt[entry].offset2 = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
	idt[entry].ist = 0;
	idt[entry].seg = 0x08; // Kernel Code Segment
	idt[entry].pdplGateType = pdplGateType;
	idt[entry].reserved = 0;
}

int arch_interrupt_init(void) {
	idtr.offset = (uint64_t)idt;
	idtr.size = sizeof(struct idt_entry) * 256 - 1;

	arch_idt_set_entry(0, isr0, INT_GATE);
	arch_idt_set_entry(1, isr1, INT_GATE);
	arch_idt_set_entry(2, isr2, INT_GATE);
	arch_idt_set_entry(3, isr3, INT_GATE);
	arch_idt_set_entry(4, isr4, INT_GATE);
	arch_idt_set_entry(5, isr5, INT_GATE);
	arch_idt_set_entry(6, isr6, INT_GATE);
	arch_idt_set_entry(7, isr7, INT_GATE);
	arch_idt_set_entry(8, isr8, INT_GATE);
	arch_idt_set_entry(9, isr9, INT_GATE);
	arch_idt_set_entry(10, isr10, INT_GATE);
	arch_idt_set_entry(11, isr11, INT_GATE);
	arch_idt_set_entry(12, isr12, INT_GATE);
	arch_idt_set_entry(13, isr13, INT_GATE);
	arch_idt_set_entry(14, isr14, INT_GATE);
	arch_idt_set_entry(15, isr15, INT_GATE);
	arch_idt_set_entry(16, isr16, INT_GATE);
	arch_idt_set_entry(17, isr17, INT_GATE);
	arch_idt_set_entry(18, isr18, INT_GATE);
	arch_idt_set_entry(19, isr19, INT_GATE);
	arch_idt_set_entry(20, isr20, INT_GATE);
	arch_idt_set_entry(21, isr21, INT_GATE);
	arch_idt_set_entry(22, isr22, INT_GATE);
	arch_idt_set_entry(23, isr23, INT_GATE);
	arch_idt_set_entry(24, isr24, INT_GATE);
	arch_idt_set_entry(25, isr25, INT_GATE);
	arch_idt_set_entry(26, isr26, INT_GATE);
	arch_idt_set_entry(27, isr27, INT_GATE);
	arch_idt_set_entry(28, isr28, INT_GATE);
	arch_idt_set_entry(29, isr29, INT_GATE);
	arch_idt_set_entry(30, isr30, INT_GATE);
	arch_idt_set_entry(31, isr31, INT_GATE);

	asm volatile("lidt %0" :: "m"(idtr));

	return 0;
}
