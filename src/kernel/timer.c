#include "timer.h"
#include "io.h"
#include "log.h"
#include "pic.h"
#include "registers.h"

static ulong tick = 0;

static void timer_cb(struct registers *regs)
{
	// do nothing :)
}

void init_timer(uint hz)
{
	add_interrupt_handler(IRQ_TO_INT(0), timer_cb);

	uint divisor = TIMER_FREQ / hz;

	kprintf("Divisor is %d\n", divisor);

	outb(0x43, 0x36);
	io_wait();
	uchar l = divisor & 0xff, h = (divisor >> 8) & 0xff;

	outb(0x40, l);
	io_wait();
	outb(0x40, h);
	io_wait();

	kprintf("Initialized timer\n");
}