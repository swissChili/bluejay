#include "gc.h"
#include "lisp.h"
#include "plat/plat.h"
#include <stdlib.h>

value_t *gc_base;
struct gc_segment *gc_last_segment = NULL;
THREAD_LOCAL static unsigned int gc_mark;
THREAD_LOCAL static unsigned long gc_runs = 0;

void __attribute__((noinline)) gc_set_base_here()
{
	// Move the current stack top address to gc_base. This works nicely because
	// it means that when another (presumably lisp) function is called right
	// after this, the stack top for it will be the same as for this function.
	asm("movl %%esp, %0" : "=g"(gc_base));
}

void gc_push_segment(void *last_arg_addr, int nretained)
{
	// base will be the address below (at higher memory than) the ret
	// pointer when lisp func is called.
	struct gc_segment *seg = malloc(sizeof(struct gc_segment) + sizeof(value_t) * nretained);

	if (last_arg_addr)
		seg->seg_start = last_arg_addr + 4;
	else
		seg->seg_start = NULL;

	seg->nretained = nretained;
	seg->prev = gc_last_segment;
	gc_last_segment = seg;

	// remember, stack looks like this:
	// ret
	// old ebp          <- current ebp points here
	// ...
	void **ebp;
	asm("movl %%ebp, %0" : "=g"(ebp));
	seg->old_ebp = *ebp; // could do this in one mov but whatever
}

void gc_pop_segment()
{
	struct gc_segment *prev = gc_last_segment->prev;
	free(gc_last_segment);
	gc_last_segment = prev;
}

void gc_prepare_call_(void *esp, int nargs)
{
	gc_last_segment->nargs = nargs;
	// esp points to its position BEFORE the lisp function is
	// called. So seg_end is that + return pointer + arguments.
	gc_last_segment->seg_end = esp + 4 + sizeof(value_t) * nargs;
}

void gc_set_retained(int index, value_t retained)
{
	gc_last_segment->retained[index] = retained;
}

void gc_set_retained(int index, value_t retained);

void _mark(value_t value, unsigned int *marked)
{
	if (heapp(value))
	{
		void *pointer = (void *)(value & ~HEAP_MASK);
		struct alloc *alloc = pointer - sizeof(struct alloc);

		// Only recursively mark if this hasn't been marked yet. i.e. prevent
		// marking circular references twice
		if (alloc->mark != gc_mark)
		{
			++*marked;

			alloc->mark = gc_mark;

			switch (alloc->type_tag)
			{
			case CONS_TAG: {
				struct cons_alloc *cons = (struct cons_alloc *)alloc;
				_mark(cons->cons.car, marked);
				_mark(cons->cons.cdr, marked);
				break;
			}
			case CLOSURE_TAG: {
				struct closure_alloc *closure = (void *)alloc;

				for (int i = 0; i < closure->closure.num_captured; i++)
				{
					_mark(closure->closure.data[i], marked);
				}

				break;
			}
			}
		}
	}
}

value_t alloc_to_value(struct alloc *a)
{
	void *val = ((void *)a) + sizeof(struct alloc);
	return (unsigned int)val | a->type_tag;
}

void _sweep()
{
	for (struct alloc *a = first_a; a;)
	{
		// fprintf(stderr, "[ GC ] %s %p pool? %d\n", (a->mark != gc_mark) ? "Unmarked" : "Marked", a, pool_alive(a->pool));
		// printval(alloc_to_value(a), 2);
		
		if (pool_alive(a->pool) || a->mark == gc_mark)
		{
			// Marked or in living pool
			a = a->next;
		}
		else
		{
			// Free and remove from allocation list
			struct alloc *p = a->prev, *n = a->next;
			del_alloc(a);

			a = n;

			if (a == first_a)
			{
				first_a = n;
			}

			if (a == last_a)
			{
				last_a = p;
			}

			if (p)
				p->next = n;

			if (n)
				n->prev = p;
		}
	}
}

struct gc_stats gc_get_stats()
{
	struct gc_stats stats = {0};

	stats.gc_runs = gc_runs;

	for (struct alloc *a = first_a; a; a = a->next)
	{
		stats.total_allocs++;
	}

	return stats;
}

void _do_gc(unsigned int esp, unsigned int ebp)
{
	value_t *esp_p = (value_t *)esp,
		*ebp_p = (value_t *)ebp;
	
	unsigned int num_marked = 0;

	gc_mark++;
	gc_runs++;

	for (struct gc_segment *seg = gc_last_segment; seg && seg->seg_start; seg = seg->prev)
	{
		// For every stack frame until the base of the stack
		while (esp_p < (value_t *)seg->seg_end)
		{
			// Walk up the stack until we reach either the frame pointer or the base
			// of the stack. Basically walk to the top of this function's stack
			// frame.
			for (; esp_p < ebp_p && esp_p < gc_base; esp_p++)
			{
				_mark(*esp_p, &num_marked);
			}

			// Set the frame pointer to the frame pointer on the stack
			ebp_p = (value_t *)*esp_p;

			// Step up two stack slots, one for the frame pointer and one for the
			// return address.
			esp_p += 2;
		}

		// skip above ret pointer
		value_t *args = seg->seg_end + 4;
		for (int i = 0; i < seg->nargs; i++)
		{
			// mark arguments
			_mark(args[i], &num_marked);
		}

		for (int i = 0; i < seg->nretained; i++)
		{
			_mark(seg->retained[i], &num_marked);
		}

		esp_p = seg->seg_start;
		ebp_p = seg->old_ebp;
	}

	_sweep();
}

void free_all()
{
	for (struct alloc *a = first_a; a;)
	{
		struct alloc *next = a->next;
		del_alloc(a);
		a = next;
//		fprintf(stderr, "a = %p\n", a);
	}
}
