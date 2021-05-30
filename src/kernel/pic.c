#include "pic.h"
#include "io.h"
#include "log.h"

#define ICW1_ICW4 0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE 0x02	/* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08		/* Level triggered (edge) mode */
#define ICW1_INIT 0x10		/* Initialization - required! */

#define ICW4_8086 0x01		 /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02		 /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08	 /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10		 /* Special fully nested (not) */

typedef void (*interrupt_handler_t)(struct registers *);

static interrupt_handler_t interrupt_handlers[256];

void pic_send_eoi(uchar interrupt)
{
	if (interrupt >= IRQ_TO_INT(8))
		outb(PIC2_COMMAND, PIC_EOI);

	outb(PIC1_COMMAND, PIC_EOI);
}

void irq_handler(struct registers regs)
{
	pic_send_eoi(regs.interrupt_number);

	if (interrupt_handlers[regs.interrupt_number])
		interrupt_handlers[regs.interrupt_number](&regs);
	else
		kprintf("Unhandled hardware interrupt: %d, called from %d\n", regs.interrupt_number, regs.eip);
}

void isr_handler(struct registers regs)
{
	if (interrupt_handlers[regs.interrupt_number])
		interrupt_handlers[regs.interrupt_number](&regs);
	else
		kprintf("Unhandled interrupt: %d, called from %d\n", regs.interrupt_number, regs.eip);
}

void add_interrupt_handler(uchar interrupt, void (*handler)(struct registers *))
{
	interrupt_handlers[interrupt] = handler;
}

void pic_remap()
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);

	memset(interrupt_handlers, 0, sizeof(interrupt_handlers));
}
