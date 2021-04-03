#include "alloc.h"
#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "multiboot.h"
#include "paging.h"
#include "syscall.h"
#include "task.h"
#include "timer.h"
#include "vfs.h"
#include "vfs_initrd.h"
#include "vga.h"

void greet()
{
	kprintf("Hello from get_task_id() = %d, get_process_id() = %d\n",
			get_task_id(), get_process_id());
}

void other_thread()
{
	greet();
	kprintf("Other thread returning...\n");
	switch_task();

	while (true)
		asm volatile("hlt");
}

int kmain(struct multiboot_info *mboot)
{
	init_paging();
	init_vga();
	init_descriptor_tables();
	init_syscall();
	vga_clear();

	init_timer(20);
	init_allocator();
	init_kbd();

#ifdef TEST_ALLOC
	test_allocator();
#endif

	// Load initrd
	struct multiboot_info mb = make_multiboot_physical(mboot);
	init_vfs();

#ifdef INITRD
	kassert(mb.mods_count, "No multiboot modules loaded!");
	kprintf("mboot->mods_addr = %d (0x%x)\n", mb.mods_addr, mb.mods_addr);
	uchar *initrd_loc = (uchar *)((uint *)mb.mods_addr)[0];

	kprintf("initrd is at 0x%x to 0x%x\n", initrd_loc);

	init_initrd_vfs(initrd_loc);
#endif

	kprintf("VFS initialized\n");

	vga_set_color(LIGHT_GREEN, BLACK);
	kprintf("Setup complete!\n");
	vga_set_color(WHITE, BLACK);

#ifdef TEST_VFS_INITRD
	kprintf("fs_readdir(\"/dev/initrd\")\n");

	struct fs_dirent dirent;
	for (int i = 0; fs_readdir(&root, i, &dirent); i++)
	{
		kprintf("name: %s, inode: %d\n", dirent.name, dirent.inode);
	}
#endif

	kprintf("initializing tasks\n");
	init_tasks();
	kprintf("\ndone initializing tasks\n");

	greet();
	spawn_thread(other_thread);
	kprintf("thread spawned\n");
	switch_task();
	kprintf("Back in main thread\n");

	asm volatile("sti");

	while (true)
		asm volatile("hlt");

	return 0xCAFEBABE;
}
