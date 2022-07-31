#pragma once

#include "lisp.h"

// I hate this
extern value_t *gc_base;

struct gc_stats
{
	// bytes currently used
	unsigned long bytes_used;
	// bytes ever freed by the GC
	unsigned long bytes_freed;
	// how many total allocations are currently alive
	unsigned long total_allocs;
	// how many times has the GC ever been run
	unsigned long gc_runs;
};

void gc_set_base_here();

value_t alloc_to_value(struct alloc *a);
void _do_gc(unsigned int esp, unsigned int ebp);
void _mark(value_t value, unsigned int *marked);
void _sweep();
void free_all();
struct gc_stats gc_get_stats();
