#include "paging.h"
#include "io.h"
#include "kint.h"
#include "log.h"
#include "pic.h"

extern uint end;
static size_t alloc_base = (size_t)&end;

/* frames bitset, 0 = free, 1 = used */
static uint *frames;
static ulong num_frames;

static uint first_page_table[1024] __attribute__((aligned(4096)));
static uint *page_directory[1024] __attribute__((aligned(4096)));

static uint ***current_directory;

void *_kmalloc(size_t size, bool align, void **phys)
{
	if (align && (alloc_base & 0xfff)) // if not yet aligned
	{
		alloc_base &= ~0xfff;
		alloc_base += 0x1000;
	}

	if (phys)
	{
		*phys = (void *)alloc_base;
	}

	size_t addr = alloc_base;
	alloc_base += size;
	return (void *)addr;
}

void *kmalloc(size_t size)
{
	return _kmalloc(size, false, NULL);
}

void *kmalloc_a(size_t size)
{
	return _kmalloc(size, true, NULL);
}

void *kmalloc_ap(size_t size, void **p)
{
	return _kmalloc(size, true, p);
}

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
	for (int i = 0; i < num_frames / BITS; i++)
	{
		/*
		 * If there are any zeroes, ~ will yield a non-zero result,
		 * meaning that there are pages free. Otherwise, check next set
		 */
		if (!~frames[i])
			continue;

		for (int j = 0; j < BITS; j++)
		{
			if ((frames[i] & 1 << j) == 0)
			{
				/* found unused frame */
				return i * BITS + j;
			}
		}
	}

	/* did not find a free frame, panic */
	kpanic("first_free_frame failed! no free frames");
}

void alloc_frame(uint *page, bool for_kernel, bool writable)
{
	if (*page >> 12)
		return; /* frame already allocated */

	uint frame = first_free_frame();
	//	kprintf("first_free_frame found %d\n", frame);
	set_frame(frame * 0x1000); /* mark as mapped */
}

void free_frame(uint page)
{
	clear_frame(page / 0x1000);
}

/* paging stuff */

void initialize_paging()
{
	for (int i = 0; i < 1024; i++)
	{
		page_directory[i] = (uint *) 0b010; // kernel, rw, not present
	}

	for (int i = 0; i < 1024; i++)
	{
		first_page_table[i] = (i * 0x1000) | 3;
	}

	page_directory[0] = (uint *) (((size_t) (uint *) first_page_table) | 3);

	kprintf("Set up page directory[0], %x\n", (uint)first_page_table);

	load_page_directory((uint) page_directory);
	enable_paging();

	add_interrupt_handler(14, page_fault);
}

void page_fault(struct registers *regs)
{
	kprintf("Page fault! eip = %d\n", regs->eip);
	kpanic("Page fault");
}
