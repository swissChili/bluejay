#include "alloc.h"
#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "paging.h"
#include "timer.h"
#include "vga.h"

int kmain(void *mboot)
{
	init_paging();
	init_vga();

	vga_clear();
	vga_set_color(LIGHT_BLUE, BLACK);
	vga_write("Hello!\nWelcome to Bluejay OS\n");
	vga_set_color(WHITE, BLACK);

	init_descriptor_tables();

	vga_set_color(LIGHT_GREEN, BLACK);
	vga_write("Setup complete!\n");
	vga_set_color(WHITE, BLACK);

	init_timer(20);
	init_allocator();
	init_kbd();

	// Test allocator

	int *one = malloc(sizeof(int));
	int *two = malloc(sizeof(int));

	*one = 1;
	*two = 2;

	int *array = malloc(sizeof(int[12]));

	for (int i = 0; i < 12; i++)
		array[i] = i;

	kprintf("Allocated one, two, array[3] = %d, %d, %d\n", *one, *two,
			array[3]);
	kprintf("[%x, %x, %x]\n", one, two, array);

	kprintf("Freeing two\n");
	free(two);
	int *four = malloc(sizeof(int));
	*four = 4;
	kprintf("Allocated four = %d (%x)\n", *four, four);

	asm volatile("sti");

	while (true)
		asm volatile("hlt");

	return 0xCAFEBABE;
}
