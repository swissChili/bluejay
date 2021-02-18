#pragma once

#include "kint.h"

struct gdt_entry
{
	ushort limit_low;
	ushort base_low;
	uchar base_middle;

	union
	{
		struct
		{
			uint a_p    : 1;
			uint a_dpl  : 2;
			uint a_dt   : 1;
			uint a_type : 4;
		} __attribute__((packed));

		uchar access;
	};

	union
	{
		struct
		{
			uint g_g    : 1;
			uint g_d    : 1;
			uint g_zero : 2; /* includes A */
			uint g_len  : 4;
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

	union
	{
		struct
		{
			uchar f_p     : 1;
			uchar f_dpl   : 2;
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


void init_descriptor_tables();
void init_idt();
void init_gdt();
