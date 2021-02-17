#include "vga.h"
#include "descriptor_tables.h"

int main(void *mboot)
{
	vga_clear();
	vga_set_color(LIGHT_BLUE, BLACK);
	vga_write("Hello!\nWelcome to Bluejay OS\n");

	vga_set_color(WHITE, BLACK);

	init_idt();

	//init_descriptor_tables();

	//asm volatile("int $0x03");
	
	return 0xCAFEBABE;
}
