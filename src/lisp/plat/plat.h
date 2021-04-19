#pragma once

#include <stdint.h>
#include <stddef.h>

/* Platform specific definitions */

// Must return an address aligned to 8 bytes
void *malloc_aligned (size_t size);
void *realloc_aligned (void *addr, size_t size);
void free_aligned (void *addr);
