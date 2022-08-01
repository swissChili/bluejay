#pragma once

#include "lisp.h"

// I hate this
extern value_t *gc_base;
extern struct gc_segment *gc_last_segment;

struct gc_segment
{
	struct gc_segment *prev;

	// The address of the first dword on the stack below the return
	// pointer for this function (i.e. when the procedure is first
	// `call`'d, this is esp+4)
	void *seg_start;

	// The address of the return pointer for the subsequently called
	// function on the stack. When the lisp function is called,
	// seg_end=esp.
	void *seg_end;

	// The number of arguments passed to the lisp function. These are
	// stored in order on the stack, starting at seg_end+4, each
	// taking 1 dword of memory.
	int nargs;

	// The value of ebp as it was when the function was first
	// invoked. This denotes the caller's stack frame.
	void *old_ebp;

	// The number of lisp values which are retained by this C
	// function.
	int nretained;
	value_t retained[];
};

#define gc_prepare_call(nargs)								   \
	{														   \
		void *__gc_segment_base;							   \
		asm("movl %%esp, %0" : "=g"(__gc_segment_base));	   \
		gc_prepare_call_(__gc_segment_base,	nargs);			   \
	}
void gc_push_segment(void *last_arg_addr, int nretained);
void gc_pop_segment();
void gc_prepare_call_(void *esp, int nargs);
void gc_set_retained(int index, value_t retained);

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
