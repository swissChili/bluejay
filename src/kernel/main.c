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
#include <dri/ata_pio/ata_pio.h>
#include <dri/pci/pci.h>
#include <dri/ide/ide.h>
#include <dri/fs/ext2/ext2.h>

void greet()
{
	kprintf("Hello from get_task_id() = %d, get_process_id() = %d\n",
			get_task_id(), get_process_id());
}

void other_thread(size_t data)
{
	kprintf("data is 0x%x\n", data);
	greet();
	kprintf("Returning from other_thread\n");

	return;
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

	asm("sti");

	init_tasks();
	pci_init();

	// Register PCI drivers
	ide_register();

	pci_load();

#ifdef TEST_THREADS
	spawn_thread(other_thread, NULL);

	greet();
#endif

#ifdef TEST_PCI
	pci_print_devices();
	pci_print_drivers();
#endif

#ifdef TEST_ATA_PIO
	test_ata_pio();
#endif

	ext2_mount(&root);

	while (true)
		asm("hlt");

	return 0xCAFEBABE;
}
