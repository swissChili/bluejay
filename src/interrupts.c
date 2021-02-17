#include "kint.h"
#include "vga.h"

struct registers
{
	uint ds;
	uint edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint interrupt_number, error_code;
	uint eip, cs, eflags, useresp, ss;
};

void isr_handler(struct registers regs)
{
	vga_write("Interrupt triggered\n");
}
