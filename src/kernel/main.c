#include "alloc.h"
#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "paging.h"
#include "timer.h"
#include "vga.h"
#include "multiboot.h"

int kmain(struct multiboot *mboot)
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

	// Load initrd
	struct multiboot mb = make_multiboot_physical(mboot);

	kassert(mb.mods_count, "No multiboot modules loaded!");
	kprintf("mboot->mods_addr = %d (0x%x)\n", mb.mods_addr, mb.mods_addr);
	uint *initrd_loc = mb.mods_addr[0],
		*initrd_end = mboot->mods_addr[1];

	kprintf("initrd is at 0x%x to 0x%x\n", initrd_loc, initrd_end);


	asm volatile("sti");

	while (true)
		asm volatile("hlt");

	return 0xCAFEBABE;
}
