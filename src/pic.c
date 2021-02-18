#include "pic.h"
#include "io.h"

void (* interrupt_handlers[256])(struct registers);

void pic_send_eoi(uchar interrupt)
{
	if (interrupt >= 40)
		outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}

void irq_handler(struct registers regs)
{
	pic_send_eoi(regs.interrupt_number);

	if (interrupt_handlers[regs.interrupt_number])
		interrupt_handlers[regs.interrupt_number](regs);
}

void add_interrupt_handler(uchar interrupt, void (* handler)(struct registers))
{
	interrupt_handlers[interrupt] = handler;
}
