#include "alloc.h"
#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "paging.h"
#include "timer.h"
#include "vga.h"
#include "vfs.h"
#include "multiboot.h"

int kmain(struct multiboot *mboot)
{
	init_paging();
	init_vga();
	init_descriptor_tables();
	vga_clear();

	init_timer(20);
	init_allocator();
	init_kbd();

#ifdef TEST_ALLOC
	test_allocator();
#endif

	// Load initrd
	struct multiboot mb = make_multiboot_physical(mboot);

	kassert(mb.mods_count, "No multiboot modules loaded!");
	kprintf("mboot->mods_addr = %d (0x%x)\n", mb.mods_addr, mb.mods_addr);
	uchar *initrd_loc = mb.mods_addr[0];

	kprintf("initrd is at 0x%x to 0x%x\n", initrd_loc);

	init_vfs();
	kprintf("VFS initialized\n");

	vga_set_color(LIGHT_GREEN, BLACK);
	vga_write("Setup complete!\n");
	vga_set_color(WHITE, BLACK);

	asm volatile("sti");

	while (true)
		asm volatile("hlt");

	return 0xCAFEBABE;
}
