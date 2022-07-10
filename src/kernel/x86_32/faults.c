#include "pic.h"
#include <log.h>

#define DECLARE_INTERRUPT(sym, name)                                           \
	static void sym##_h(struct registers *regs)                                \
	{                                                                          \
		kprintf(ERROR "Fault " name ": eip = 0x%x, err = 0x%x\n", regs->eip,   \
				regs->error_code);                                             \
		asm volatile("cli");                                                   \
		kpanic(name);                                                          \
	}

#define ADD_INTERRUPT(sym, num) add_interrupt_handler(num, sym##_h)

DECLARE_INTERRUPT(gp, "#GP")
DECLARE_INTERRUPT(pf, "#PF")

void init_faults()
{
	ADD_INTERRUPT(gp, 13);
	ADD_INTERRUPT(pf, 14);
}
