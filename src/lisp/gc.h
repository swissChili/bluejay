#pragma once

#include "lisp.h"

// I hate this
extern value_t *gc_base;

struct gc_skipped_segment
{
	// Top of stack (low memory)
	void *top;
	// Bottom of stack (high memory)
	void *bottom;

	value_t mark[8];
	int num_marked;
};

extern struct gc_skipped_segment gc_segments[64];
extern int gc_current_segment;

void gc_set_base_here();

value_t alloc_to_value(struct alloc *a);
unsigned int _do_gc(unsigned int esp, unsigned int ebp);
void _mark(value_t value, unsigned int *marked);
unsigned int _sweep();
void free_all();

// varargs should be a series of value_t's, followed by nil. Call this
// immediately in any function that is called by Lisp and may call
// other Lisp functions.
void gc_skip(void *last_arg, ...);
void gc_resume();
/// @param n Number of non-GC arguments passed to function
void gc_resumen(int n);
void gc_endskip();
void gc_pushmark(value_t val);
void gc_popmark();
