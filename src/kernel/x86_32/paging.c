#include "paging.h"
#include "alloc.h"
#include "io.h"
#include "kint.h"
#include "log.h"
#include "pic.h"

/**
 * NOTE: In order to understand this code you should have the paging
 * section of the Intel IA-32 and 64 manual volume 3 open. Sadly I
 * have littered this with magic numbers that you will need to consult
 * the manual to understand.
 * TODO: Fix this!
 */

#define NUM_FRAMES 0xffffffff / 0x1000 / 32
/* frames bitset, 0 = free, 1 = used */
static uint frames[NUM_FRAMES];

uint kernel_page_directory[1024] __attribute__((aligned(4096)));


/* frame utils */

#define BITS 32

static void set_frame(size_t frame_addr)
{
	uint frame = frame_addr / 0x1000; // page aligned
	frames[frame / BITS] |= 1 << (frame % BITS);
}

static void clear_frame(size_t frame_addr)
{
	uint frame = frame_addr / 0x1000; // page aligned
	frames[frame / BITS] &= ~(1 << (frame % BITS));
}

static uint first_free_frame()
{
	for (int i = 0; i < NUM_FRAMES / BITS; i++)
	{
		/*
		 * If there are any zeroes, ~ will yield a non-zero result,
		 * meaning that there are pages free. Otherwise, check next set
		 */
		if (!~frames[i])
			continue;

		for (int j = 0; j < BITS; j++)
		{
			if ((frames[i] & (1 << j)) == 0)
			{
				/* found unused frame */
				uint frame = i * BITS + j;
				kprintf(DEBUG "first_free_frame returning %d\n", frame);

				return frame;
			}
		}
	}

	/* did not find a free frame, panic */
	kpanic("first_free_frame failed! no free frames");
}

void alloc_frame(uint *page_table_entry, bool user, bool writable)
{
	if (*page_table_entry & 1)
		return; /* frame already allocated */

	uint frame = first_free_frame();
	//	kprintf(DEBUG "first_free_frame found %d\n", frame);
	set_frame(frame * 0x1000); /* mark as mapped */
	*page_table_entry = frame | 1 | writable << 1 | user << 2;
}

void free_frame(uint page)
{
	clear_frame(page / 0x1000);
}

void map_4mb(uint *dir, size_t virt_start, size_t phys_start, bool user,
			 bool rw)
{
	uint table = virt_start >> 22;

	for (uint i = 0; i < 1024 * 0x1000; i += 0x1000)
	{
		set_frame(i);
	}

	dir[table] = 0b10000011;
}

uint *get_or_create_table(uint *dir, uint table, bool user, bool rw)
{
	// If used AND NOT 4mb page (see figure 4-4, page 114 of Intel
	// manual volume 3)
	// bit 0 = used; bit 7 = 4MB?
	if (dir[table] & 1 && dir[table] ^ (1 << 7))
	{
		// 12 LSBs are metadata
		return (uint *)(size_t)PHYS_TO_VIRT((dir[table] & ~0xfff));
	}

	uint *page_table = malloc(sizeof(uint[1024]));
	dir[table] = VIRT_TO_PHYS(page_table) | 1 | rw << 1 | user << 2;
	return page_table;
}

void unmap_all_frames(uint page_table_p)
{
	uint *table = (uint *)PHYS_TO_VIRT(page_table_p);

	for (int i = 0; i < 1024; i++)
	{
		if (table[i] & 1)
		{
			clear_frame(table[i] >> 12);
		}
	}
}

void destroy_page_table_if_exists(uint *dir, uint table)
{
	// If used AND NOT 4mb page
	if (dir[table] & 1 && dir[table] ^ 1 << 7)
	{
		unmap_all_frames(dir[table] >> 12);
		free((void *)PHYS_TO_VIRT(dir[table] >> 12));
	}
}

void unmap_page(uint *dir, void *virt)
{
	uint page = ((size_t)virt / 0x1000) % 1024;
	uint *table = get_or_create_table(dir, (size_t)virt >> 22, false, false);

	table[page] = 0;
}

// TODO: This entire function seems wrong.
void map_page_to(uint *dir, void *virt, void *frame_p, bool writable, bool user)
{
	uint page = ((size_t)virt / 0x1000) % 1024;
	uint *table = get_or_create_table(dir, (size_t)virt >> 22, false, false);

	// TODO: is the xor here correct?
	table[page] = (((uint)frame_p) ^ 0xfff) | 1 | writable << 1 | user << 2;
}

void alloc_kernel_page(uint *virt)
{
	alloc_page(kernel_page_directory, virt);
}

void alloc_page(uint *dir, uint *virt)
{
	// Page number % pages per table
	uint page = ((size_t)virt / 0x1000) % 1024;
	uint *table = get_or_create_table(dir, (size_t)virt >> 22, false, false);
	// kprintf(DEBUG "table = 0x%x (virt)\n", table);
	// kprintf(DEBUG "dir entry = 0x%x\n", dir[(size_t)virt >> 22]);

	alloc_frame(&table[page], false, false);

	// kprintf(DEBUG "alloc_page table[page] = %d (0x%x)\n", table[page], table[page]);
	return;
}

void alloc_kernel_page_range(uint *from, uint *to)
{
	uint f = (size_t)from / 0x1000, t = (size_t)to / 0x1000;

	do
	{
		alloc_kernel_page((uint *)(size_t)t);
		t += 0x1000; // next page
	} while (f < t);
}

void map_page(uint *dir, size_t virt_start, bool user, bool rw)
{
	// Page number % pages per table
	uint page = (virt_start / 0x1000) % 1024;
	uint table = virt_start >> 22;
	uint frame = first_free_frame();

	// If 4mb map OR (maps to table AND table maps page)
	if ((dir[table] & 1 && dir[table] & 1 << 7) ||
		(dir[table] >> 12 && ((uint *)(size_t)dir[table])[page] & 1))
	{
		return;
	}

	set_frame(frame);
	uint *t = get_or_create_table(dir, table, user, rw);
	alloc_frame(t + page, user, rw);
}

/* paging stuff */

uint *new_page_directory_v()
{
	// Only call this AFTER allocator + paging are initialized!
	uint *dir = malloc(1024 * 4);
	map_4mb(kernel_page_directory, (size_t)KERNEL_VIRTUAL_BASE, 0, false,
			false);

	return dir;
}

void free_page_directory_v(uint *dir_v)
{
	for (int i = 0; i < 1024; i++)
	{
		destroy_page_table_if_exists(dir_v, i);
	}

	free(dir_v);
}

void init_paging()
{
	memset(kernel_page_directory, 0, 1024 * 4);
	map_4mb(kernel_page_directory, (size_t)KERNEL_VIRTUAL_BASE, 0, false,
			false);

	load_page_directory(VIRT_TO_PHYS(kernel_page_directory));
}

#ifdef TEST_PAGING
void test_paging()
{
	// a random page base address
	uint *base = (uint *)0xFFFFFE000;

	kprintf(INFO "Allocating page (expect frame 1024)\n");
	alloc_page(kernel_page_directory, base);

	kprintf(INFO "Writing 10 words to page\n");
	for (int i = 0; i < 10; i++)
	{
		base[i] = i;
	}
}
#endif
