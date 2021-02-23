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
static uint page_directory[1024] __attribute__((aligned(4096)));

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

void map_4mb(size_t virt_start, size_t phys_start, bool user, bool rw)
{
	uint page = virt_start / 0x1000;
	uint table = virt_start >> 22;

	for (uint i = 0; i < 1024 * 0x1000; i += 0x1000)
	{
		set_frame(page + i);
	}

	page_directory[table] = 0b10000011;
}

/* paging stuff */

void init_paging()
{
	memset(page_directory, 0, 1024 * 4);
	page_directory[KERNEL_PAGE_NUMBER] = 0b10000011;

	load_page_directory((uint)page_directory - 0xC0000000);
	add_interrupt_handler(14, page_fault);
}

void page_fault(struct registers *regs)
{
	kprintf("Page fault! eip = %d\n", regs->eip);
	kpanic("Page fault");
}