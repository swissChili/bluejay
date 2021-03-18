#include "task.h"
#include "alloc.h"
#include "io.h"
#include "paging.h"

uint clone_page_table(uint table)
{
	// is this table a 4mb mapping OR is it not mapped?
	// See Intel manual volume 3 figure 4-4.

	if (table & 1 << 7 || !(table & 1))
	{
		// if so, return as is, 4mb mappings only used by kernel and should
		// be mapped to the same location
		return table;
	}
	else
	{
		// else, look for the used pages in this table, copy them to a new
		// frame, and store them in a new table, then return that table
		// TODO: add copy on write

		uint new_p;
		uint *new = kmalloc_ap(sizeof(uint[1024]), (void **)&new_p);

		// We don't care about the lower 12 bits, just want the address
		uint *old_virt = PHYS_TO_VIRT((uint *)(table ^ 0xfff));

		for (int i = 0; i < 1024; i++)
		{
			if (old_virt[i] & 1)
			{
				// If mapped

				// again, don't care about bottom 12 bits
				uchar *old_page = PHYS_TO_VIRT(old_virt[i] ^ 0xfff);

				alloc_frame(&new[i], BOOL(old_virt[i] & 1 << 2),
							BOOL(old_virt[i] & 1 << 1));

				uchar *new_page = PHYS_TO_VIRT(new[i] ^ 0xfff);
				memcpy(new_page, old_page, 0x1000);
			}
		}

		return new_p | (table & 0xfff); // same lower 12 bits
	}
}

uint *clone_page_directory(uint *dir)
{
	uint new_p;
	uint *new = kmalloc_ap(sizeof(uint[1024]), (void **)&new_p);

	for (int i = 0; i < 1024; i++)
	{
		new[i] = clone_page_table(dir[i]);
	}

	return new;
}
