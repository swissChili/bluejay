#include "multiboot.h"
#include "io.h"
#include "log.h"

#define TO_VIRT(val, t) val = (t)((size_t)val + 0xc0000000u)

// old should be a VIRTUAL address
struct multiboot make_multiboot_physical(struct multiboot *old)
{
	struct multiboot mb;
	memcpy(&mb, old, sizeof(mb));

	// Make modules physical
	TO_VIRT(mb.mods_addr, uint **);
	TO_VIRT(mb.cmdline, char *);

	kprintf("mb.mods_addr = %d, 0x%x\n", mb.mods_addr, mb.mods_addr);
	kassert((size_t)mb.mods_addr > 0xc0000000, "mb.mods_addr PHYSICAL");
	for (int i = 0; i < mb.mods_count + 1; i++)
	{
		TO_VIRT(mb.mods_addr[i], uchar *);
	}

	return mb;
}
