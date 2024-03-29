#include "timer.h"
#include "io.h"
#include "log.h"
#include "pic.h"
#include "registers.h"
#include "task.h"

static void timer_cb(struct registers *regs)
{
	if (tasks_initialized)
	{
		// Preemptive multitasking!
		switch_task(*regs);
	}
}

void init_timer(uint hz)
{
	add_interrupt_handler(IRQ_TO_INT(0), timer_cb);

	uint divisor = TIMER_FREQ / hz;

	outb(0x43, 0x36);
	uchar l = divisor & 0xff, h = (divisor >> 8) & 0xff;

	outb(0x40, l);
	outb(0x40, h);
}
