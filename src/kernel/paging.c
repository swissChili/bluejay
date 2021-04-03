#include "paging.h"
#include "alloc.h"
#include "io.h"
#include "kint.h"
#include "log.h"
#include "pic.h"

#define NUM_FRAMES 0xffffffff / 0x1000 / 32
/* frames bitset, 0 = free, 1 = used */
static uint frames[NUM_FRAMES];

static uint first_page_table[1024] __attribute__((aligned(4096)));
uint kernel_page_directory[1024] __attribute__((aligned(4096)));

/* frame utils */

#define BITS 32

static void set_frame(size_t frame_addr)
{
	uint frame = frame_addr / 0x1000; // page aligned
	frames[frame / BITS] |= 1 << (frame % BITS);
}

static bool test_frame(size_t frame_addr)
{
	uint frame = frame_addr / 0x1000; // page aligned
	return frames[frame / BITS] & 1 << (frame % BITS);
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
				kprintf("first_free_frame returning %d\n", frame);
//				kpanic("asdf");
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
	//	kprintf("first_free_frame found %d\n", frame);
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
	uint page = virt_start / 0x1000;
	uint table = virt_start >> 22;

	for (uint i = 0; i < 1024 * 0x1000; i += 0x1000)
	{
		set_frame(i);
	}

	dir[table] = 0b10000011;
}

uint *get_or_create_table(uint *dir, uint table, bool user, bool rw)
{
	if (dir[table] >> 12)
	{
		return (uint *)(size_t)PHYS_TO_VIRT((dir[table] ^ 0xfff));
	}

	uint *page_table = kmalloc_a(sizeof(uint[1024]));
	dir[table] = (uint)page_table | 1 | rw << 1 | user << 2;
	return page_table;
}

void unmap_page(uint *dir, void *virt)
{
	uint page = ((size_t)virt / 0x1000) % 1024;
	uint *table = get_or_create_table(dir, (size_t)virt >> 22, false, false);

	table[page] = 0;
}

void map_page_to(uint *dir, void *virt, void *frame_p, bool writable, bool user)
{
	uint page = ((size_t)virt / 0x1000) % 1024;
	uint *table = get_or_create_table(dir, (size_t)virt >> 22, false, false);

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

	alloc_frame(&table[page], false, false);

	kprintf("alloc_page table[page] = %d (0x%x)\n", table[page], table[page]);
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

void init_paging()
{
	memset(kernel_page_directory, 0, 1024 * 4);
	map_4mb(kernel_page_directory, (size_t)KERNEL_VIRTUAL_BASE, 0, false,
			false);

	load_page_directory((uint)kernel_page_directory - 0xC0000000);
	add_interrupt_handler(14, page_fault);
}

void page_fault(struct registers *regs)
{
	kprintf("Page fault! eip = %d\n", regs->eip);
	kpanic("Page fault");
}
