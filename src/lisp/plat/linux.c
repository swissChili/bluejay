#include "plat.h"
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

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

void *link (dasm_State **Dst)
{
	size_t size;
	void *buf;

	dasm_link (Dst, &size);
	buf = mmap (0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	dasm_encode (Dst, buf);

	mprotect (buf, size, PROT_READ | PROT_EXEC);
	return buf;
}
