#include "alloc.h"
#include "io.h"
#include "kheap.h"
#include "kint.h"
#include "log.h"
#include "paging.h"

extern uint end;
static size_t palloc_base = (size_t)&end;
static size_t malloc_base = (size_t)&end + 0x8000;

#define HEADER_SIZE sizeof(struct heap_alloc_header)
#define FOOTER_SIZE sizeof(struct heap_alloc_footer)

static struct min_heap heap = {0};

void *_kmalloc(size_t size, bool align, void **phys)
{
	if (align && (palloc_base & 0xfff)) // if not yet aligned
	{
		palloc_base &= ~0xfff;
		palloc_base += 0x1000;
	}

	if (phys)
	{
		*phys = (void *)VIRT_TO_PHYS((void *)palloc_base);
	}

	size_t addr = palloc_base;
	palloc_base += size;

	if (palloc_base >= malloc_base)
	{
		kpanic("fatal error: placeholder kmalloc has overrun malloc() memory,"
			   " cannot recover.");
	}

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

// Proper allocators

void init_allocator()
{
	heap.size = 1;
	int size = 0xC0400000 - malloc_base;
	heap.elements[0] = (struct heap_entry){
		.key = size,
		.address = malloc_base,
	};
	memset((void *)malloc_base, 0, size);

	struct heap_alloc_header *h = (struct heap_alloc_header *)malloc_base;
	h->magic = HEAP_MAGIC;
	h->size = size;
	h->allocated = false;
}

void *malloc(size_t size)
{
	bool ok;
	size_t full_size = size + HEADER_SIZE + FOOTER_SIZE;
	int i;

	struct heap_entry e = heap_lookup_min(&heap, full_size, &ok, false, &i);

	if (ok)
	{
		// Found smallest hole
		struct heap_alloc_header *h = (struct heap_alloc_header *)e.address;

		kassert(!h->allocated,
				"Gap already allocated (this should never happen)");

		size_t old_size = h->size;

		if (full_size == old_size)
		{
			// Completely used, no need to change anything!
			heap_delete(&heap, i);
		}
		else
		{
			// If there isn't very much space left
			size_t new_size = old_size - full_size;
			if (new_size <= HEADER_SIZE + FOOTER_SIZE + 8)
			{
				full_size = old_size;
				heap_delete(&heap, i);
			}
			else
			{
				struct heap_alloc_footer *old_f =
					(struct heap_alloc_footer *)(e.address + old_size -
												 FOOTER_SIZE);

				// Else create a new header
				size_t new_header_addr = e.address + full_size;
				struct heap_alloc_header *h =
					(struct heap_alloc_header *)new_header_addr;

				h->size = new_size;
				old_f->size = new_size;

				heap_decrease(&heap, i,
							  (struct heap_entry){
								  .key = new_size,
								  .address = new_header_addr,
							  });
			}

			struct heap_alloc_footer *f =
				(struct heap_alloc_footer *)(e.address + full_size -
											 FOOTER_SIZE);

			h->allocated = true;
			h->magic = HEAP_MAGIC;
			h->size = full_size;
			f->size = h->size;
		}

		return (void *)(e.address + HEADER_SIZE);
	}
	else
	{
		// We need more memory :L
		kpanic("Whoops, malloc ran out of memory");
	}
}

void free(void *mem)
{
	if (!mem)
		return; // freeing NULL ptr

	struct heap_alloc_header *base =
		(struct heap_alloc_header *)((size_t)mem - HEADER_SIZE);

	if (base->magic != HEAP_MAGIC)
	{
		kpanic("Freeing memory not allocated with malloc()");
	}

	// Check free block before this one

	struct heap_alloc_footer *prev_f =
		(struct heap_alloc_footer *)((size_t)mem - HEADER_SIZE - FOOTER_SIZE);

	// Header of block before this one
	struct heap_alloc_header *prev_h =
		(struct heap_alloc_header *)((size_t)prev_f - prev_f->size +
									 FOOTER_SIZE);

	// Header of block after this one
	struct heap_alloc_header *next_h =
		(struct heap_alloc_header *)((size_t)mem - HEADER_SIZE + base->size);

	size_t size = base->size;
	size_t start = (size_t)base;

	if (prev_h->magic == HEAP_MAGIC && !prev_h->allocated)
	{
		size += prev_h->size;
		start = (size_t)prev_h;
	}
	if (next_h->magic == HEAP_MAGIC && !next_h->allocated)
	{
		size += next_h->size;
	}

	struct heap_alloc_header *h = (struct heap_alloc_header *)start;
	h->allocated = false;
	h->magic = HEAP_MAGIC;
	h->size = size;

	// Add entry into heap

	struct heap_entry entry = {
		.key = size,
		.address = start,
	};

	heap_insert(&heap, entry);
}

void *realloc(void *mem, size_t size)
{
	if (!mem)
		return NULL; // freeing NULL ptr

	struct heap_alloc_header *base =
		(struct heap_alloc_header *)((size_t)mem - HEADER_SIZE);
	struct heap_alloc_header *next =
		(struct heap_alloc_header *)((size_t)base + base->size);

	if (!next->allocated &&
		next->size + base->size - HEADER_SIZE - FOOTER_SIZE >= size)
	{
		// Okay, we can just expand this block
		// Actually, need to check if there is enough space remaining for an
		// additional block, otherwise, just add that memory to this block (
		// same as is done in malloc )

		struct heap_alloc_footer *f;

		size_t remaining =
			base->size + next->size - size - HEADER_SIZE - FOOTER_SIZE;

		struct heap_entry old_entry = {
			.key = next->size,
			.address = (size_t)next,
		};

		if (remaining <= HEADER_SIZE + FOOTER_SIZE + 8)
		{
			// Just join this into the same memory chunk
			f = (struct heap_alloc_footer *)(next + next->size - FOOTER_SIZE);

			heap_delete_entry(&heap, old_entry);
		}
		else
		{
			f = mem + size;
			struct heap_alloc_header *new_h =
				(struct heap_alloc_header *)(f + FOOTER_SIZE);

			struct heap_alloc_footer *new_f =
				(struct heap_alloc_footer *)(new_h + remaining - FOOTER_SIZE);

			new_h->allocated = false;
			new_h->size = new_f->size = remaining;
			new_h->magic = HEAP_MAGIC;

			struct heap_entry entry = {
				.key = remaining,
				.address = (size_t)new_h,
			};

			heap_decrease_entry(&heap, old_entry, entry);
		}

		size_t full_size = (size_t)f - (size_t)base + FOOTER_SIZE;

		f->size = full_size;
		base->size = full_size;
		base->magic = HEAP_MAGIC;
		base->allocated = true;

		return mem;
	}
	else
	{
		void *new = malloc(size);
		if (!new)
			return new;

		memcpy(new, mem, base->size - HEADER_SIZE - FOOTER_SIZE);
		free(mem);

		return new;
	}
}

void test_allocator()
{
	int *one = malloc(sizeof(int));
	int *two = malloc(sizeof(int));

	*one = 1;
	*two = 2;

	int *array = malloc(sizeof(int[12]));

	for (int i = 0; i < 12; i++)
		array[i] = i;

	kprintf(DEBUG "Allocated one, two, array[3] = %d, %d, %d\n", *one, *two,
			array[3]);
	kprintf(DEBUG "[%x, %x, %x]\n", one, two, array);

	kprintf(DEBUG "Freeing two\n");
	free(two);
	int *four = malloc(sizeof(int));
	*four = 4;
	kprintf(DEBUG "Allocated four = %d (%x)\n", *four, four);
}
