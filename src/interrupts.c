#include "kint.h"
#include "vga.h"
#include "registers.h"

void isr_handler(struct registers regs)
{
	vga_write("Interrupt triggered! ");
	vga_putx(regs.interrupt_number);
	vga_write("\n");
}
