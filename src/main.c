#include "vga.h"
#include "log.h"
#include "descriptor_tables.h"

int kmain(void *mboot)
{
	vga_clear();
	vga_set_color(LIGHT_BLUE, BLACK);
	vga_write("Hello!\nWelcome to Bluejay OS\n");
	vga_set_color(WHITE, BLACK);

	init_descriptor_tables();

	vga_set_color(LIGHT_GREEN, BLACK);
	vga_write("Setup complete!\n");
	vga_set_color(WHITE, BLACK);

	kassert(0, "should fail");

	while (1)
	{}
	
	return 0xCAFEBABE;
}
