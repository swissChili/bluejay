#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "timer.h"
#include "vga.h"
#include "paging.h"

int kmain(void *mboot)
{
	init_paging();

	vga_clear();
	vga_set_color(LIGHT_BLUE, BLACK);
	vga_write("Hello!\nWelcome to Bluejay OS\n");
	vga_set_color(WHITE, BLACK);

	init_descriptor_tables();

	vga_set_color(LIGHT_GREEN, BLACK);
	vga_write("Setup complete!\n");
	vga_set_color(WHITE, BLACK);

	init_timer(20);

	init_kbd();
	asm volatile("sti");


#ifdef TEST_PAGE_FAULT
	kprintf("Causing page fault:\n");

	volatile uint *ptr = (uint *) 0xa0000000;
	volatile uint cause_page_fault = *ptr;

	kprintf("Should have caused page fault: %d...\n", cause_page_fault);
#endif

	while (true)
		asm volatile("hlt");

	return 0xCAFEBABE;
}
