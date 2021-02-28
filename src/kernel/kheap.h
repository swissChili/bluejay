#pragma once

#include "kint.h"

#define HEAP_SIZE 4096
#define HEAP_MAGIC 0xCAFEBABE

struct heap_entry
{
	uint key;
	size_t address;
};

struct min_heap
{
	struct heap_entry elements[HEAP_SIZE];
	uint size;
};

struct heap_alloc_header
{
	uint magic;
	bool allocated;
	// size = size from beginning of header to end of footer
	size_t size;
};

struct heap_alloc_footer
{
	// size = size from beginning of header to end of footer
	size_t size;
};

int heap_parent(int i);
int heap_left(int i);
int heap_right(int i);
void heap_delete(struct min_heap *heap, int i);
void heap_insert(struct min_heap *heap, struct heap_entry e);
void heap_decrease(struct min_heap *heap, int k, struct heap_entry to);
void heap_decrease_entry(struct min_heap *heap, struct heap_entry from,
						 struct heap_entry to);
void heap_delete_entry(struct min_heap *heap, struct heap_entry e);

struct heap_entry heap_lookup_min(struct min_heap *heap, int min, bool *ok,
								  bool delete, int *i);
