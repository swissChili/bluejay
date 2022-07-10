#pragma once

#include "kint.h"

struct gdt_entry
{
	ushort limit_low;
	ushort base_low;
	uchar base_middle;

	union {
		struct
		{
			uint a_p : 1;
			uint a_dpl : 2;
			uint a_dt : 1;
			uint a_type : 4;
		} __attribute__((packed));

		uchar access;
	};

	union {
		struct
		{
			uint g_g : 1;
			uint g_d : 1;
			uint g_zero : 2; /* includes A */
			uint g_len : 4;
		} __attribute__((packed));

		uchar granularity;
	};

	uchar base_high;
} __attribute__((packed));

struct gdt_pointer
{
	/* Upper 16 bits of selector limits */
	ushort limit;
	/* first struct gdt_entry */
	uint base;
} __attribute__((packed));

struct idt_entry
{
	ushort base_low;
	ushort selector;
	uchar zero;

	union {
		struct
		{
			uchar f_p : 1;
			uchar f_dpl : 2;
			uchar f_const : 5;
		} __attribute__((packed));

		uchar flags;
	};

	ushort base_high;
} __attribute__((packed));

#define IDT_F_CONST 0b00110

struct idt_pointer
{
	ushort limit;
	uint base;
} __attribute__((packed));

// We don't use hardware task switching, but we need a TSS entry
// anyway.
struct __attribute__((packed)) tss_entry
{
	// Previous TSS. Unused.
	uint prev_tss;   
	// Kernel stack pointer.
	uint esp0;       
	// Kernel stack segment.
	uint ss0;        
	// Unused
	uint esp1;       
	uint ss1;
	uint esp2;
	uint ss2;
	uint cr3;
	uint eip;
	uint eflags;
	uint eax;
	uint ecx;
	uint edx;
	uint ebx;
	uint esp;
	uint ebp;
	uint esi;
	uint edi;
	// The value to load into ES when we change to kernel mode.
	uint es;         
	// The value to load into CS when we change to kernel mode.
	uint cs;         
	// The value to load into SS when we change to kernel mode.
	uint ss;         
	// The value to load into DS when we change to kernel mode.
	uint ds;         
	// The value to load into FS when we change to kernel mode.
	uint fs;         
	// The value to load into GS when we change to kernel mode.
	uint gs;         
	// Unused...
	uint ldt;        
	ushort trap;
	ushort iomap_base;
};

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

// Syscall
extern void isr128();
// Tasking setup
extern void isr129();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void init_descriptor_tables();
void init_idt();
void init_gdt();

/// Set the stack to be used for Kernel-mode interrupt routines
void set_kernel_interrupt_stack(void *stack);

extern struct tss_entry tss_entry;
