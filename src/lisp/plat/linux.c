#include "plat.h"
#include <stdlib.h>
#include <string.h>

void *malloc_aligned (size_t size)
{
	void *mem = malloc (size + 8 + sizeof (void *) * 2);
	void **aligned_ptr = (void **) ((uintptr_t) (mem + 8 + sizeof (void *)) & ~7);
	aligned_ptr[ -1 ] = mem;
	aligned_ptr[ -2 ] = (void *) size;
	return aligned_ptr;
}

void *realloc_aligned (void *addr, size_t size)
{
	void *mem = malloc (size + 8 + sizeof (void *) * 2);
	void **aligned_ptr = (void **) ((uintptr_t) (mem + 8 + sizeof (void *)) & ~7);
	aligned_ptr[ -1 ] = mem;

	memcpy (aligned_ptr, addr, (uintptr_t) aligned_ptr[ -2 ]);
	
	return aligned_ptr;
}

void free_aligned (void *addr)
{
	void **ptr = (void **)addr;
	free (ptr[ -1 ]);
}
