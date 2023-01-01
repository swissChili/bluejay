#include "alloc.h"
#include "descriptor_tables.h"
#include "io.h"
#include "log.h"
#include "multiboot.h"
#include "paging.h"
#include "sync.h"
#include "syscall.h"
#include "task.h"
#include "timer.h"
#include "vfs.h"
#include "vfs_initrd.h"
#include "vga.h"
#include <dri/ata_pio/ata_pio.h>
#include <dri/fs/ext2/ext2.h>
#include <dri/ide/ide.h>
#include <dri/pci/pci.h>

void greet()
{
	kprintf(DEBUG "Hello from get_task_id() = %d, get_process_id() = %d\n",
			get_task_id(), get_process_id());
}

void other_thread(size_t data)
{
	kprintf(DEBUG "data is 0x%x\n", data);
	greet();
	kprintf(DEBUG "Returning from other_thread\n");

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

	init_vfs();

#ifdef INITRD
	// Load initrd
	struct multiboot_info mb = make_multiboot_physical(mboot);

	kassert(mb.mods_count, "No multiboot modules loaded!");
	kprintf(DEBUG "mboot->mods_addr = %d (0x%x)\n", mb.mods_addr, mb.mods_addr);
	uchar *initrd_loc = (uchar *)((uint *)mb.mods_addr)[0];

	kprintf(DEBUG "initrd is at 0x%x to 0x%x\n", initrd_loc);

	init_initrd_vfs(initrd_loc);
	kprintf(OKAY "VFS initialized\n");
#endif

	asm("sti");

	init_tasks();
	kprintf(OKAY "Tasks initialized\n");
	init_sync();

#ifdef TEST_PAGING
	test_paging();
#endif

#ifdef TEST_THREADS
	kprintf(DEBUG "Spawning test thread\n");
	spawn_thread(other_thread, NULL);

	greet();
#endif

	pci_init();
	kprintf(OKAY "PCI initialized\n");

	// Register PCI drivers
	ide_register();

	pci_load();

	kprintf(OKAY "Loaded PCI device drivers\n");

#ifdef TEST_PCI
	pci_print_devices();
	pci_print_drivers();
#endif

#ifdef TEST_ATA_PIO
	test_ata_pio();
#endif

	if (ext2_valid_filesystem())
	{
		kprintf(OKAY "EXT2 filesystem is valid\n");
		ext2_mount(&root);
	}
	else
	{
		kprintf(
			WARN
			"Filesystem is not a valid EXT2 format, only EXT2 is supported\n");
	}

	kprintf(INFO "fs_readdir(\"/\") mnt=%p\n", root.mount);

	struct fs_dirent dirent;
	for (int i = 0; fs_readdir(&root, i, &dirent); i++)
	{
		kprintf(INFO "name: %s, inode: %d\n", dirent.name, dirent.inode);
	}

	while (true)
		asm("hlt");

	return 0xCAFEBABE;
}
