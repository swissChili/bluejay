#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "pic.h"
#include "vga.h"

extern void gdt_flush(uint gdt);
extern void idt_flush(uint idt);

static void gdt_set_gate(uint i, uint base, uint limit, uchar access,
						 uchar gran);
static void idt_set_gate(uchar num, uint base, ushort selector, uchar flags);

struct gdt_entry gdt_entries[5];
struct gdt_pointer gdt_pointer;

struct idt_entry idt_entries[256];
struct idt_pointer idt_pointer;

static void (*isrs[32])() = {
	isr0,  isr1,  isr2,	 isr3,	isr4,  isr5,  isr6,	 isr7,	isr8,  isr9,  isr10,
	isr11, isr12, isr13, isr14, isr15, isr16, isr17, isr18, isr19, isr20, isr21,
	isr22, isr23, isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

static void (*irqs[16])() = {irq0,	irq1,  irq2,  irq3, irq4,  irq5,
							 irq6,	irq7,  irq8,  irq9, irq10, irq11,
							 irq12, irq13, irq14, irq15};

extern void (*interrupt_handlers[256])(struct registers);

void init_gdt()
{
	vga_write("Initializing GDT...\n");
	gdt_pointer.limit = sizeof(struct gdt_entry) * 5 - 1;
	gdt_pointer.base = (uint)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);		// Null segment
	gdt_set_gate(1, 0, ~0, 0x9a, 0xcf); // Code segment
	gdt_set_gate(2, 0, ~0, 0x92, 0xcf); // Data segment
	gdt_set_gate(3, 0, ~0, 0xfa, 0xcf); // User mode code segment
	gdt_set_gate(4, 0, ~0, 0xf2, 0xcf); // User mode data segment

	for (volatile uint i = 0; i < 0x1000; i++)
	{
	} // waste some time, for some reason this helps

	gdt_flush((uint)&gdt_pointer);

	vga_write("GDT Initialized\n");
}

static void gdt_set_gate(uint i, uint base, uint limit, uchar access,
						 uchar gran)
{
	struct gdt_entry *e = &gdt_entries[i];

	e->base_low = base & 0xffff;
	e->base_middle = (base >> 16) & 0xff;
	e->base_high = (base >> 24) & 0xff;

	e->limit_low = limit & 0xffff;
	e->granularity = ((limit >> 16) & 0x0f) | (gran & 0xf0);

	e->access = access;
}

void init_idt()
{
	idt_pointer.limit = sizeof(struct idt_entry) * 256 - 1;
	idt_pointer.base = (uint)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entries));

	// Remap PIC
	pic_remap();

	vga_set_color(CYAN, BLACK);
	for (int i = 0; i < 16; i++)
	{
		idt_set_gate(IRQ_TO_INT(i), (uint)irqs[i], 0x08, 0x8e);
	}
	vga_set_color(WHITE, BLACK);

	for (int i = 0; i < 32; i++)
	{
		idt_set_gate(i, (uint)isrs[i], 0x08, 0x8e);
	}

	idt_flush((uint)&idt_pointer);

	vga_write("IDT Initialized!\n");
}

static void idt_set_gate(uchar num, uint base, ushort selector, uchar flags)
{
	struct idt_entry *e = &idt_entries[num];

	e->base_low = base & 0xffff;
	e->base_high = (base >> 16) & 0xffff;

	e->selector = selector;
	e->zero = 0;
	e->flags = flags /* | 0x60 */;
}

void init_descriptor_tables()
{
	init_gdt();
	init_idt();
	memset(interrupt_handlers, 0, sizeof(interrupt_handlers));
}
