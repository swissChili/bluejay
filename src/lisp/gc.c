#include "gc.h"
#include "lisp.h"
#include "plat/plat.h"

value_t *gc_base;
THREAD_LOCAL static unsigned int gc_mark;
THREAD_LOCAL static unsigned long gc_runs = 0;

void __attribute__((noinline)) gc_set_base_here()
{
	// Move the current stack top address to gc_base. This works nicely because
	// it means that when another (presumably lisp) function is called right
	// after this, the stack top for it will be the same as for this function.
	asm("movl %%esp, %0" : "=g"(gc_base));
}

void _mark(value_t value, unsigned int *marked)
{
	if (heapp(value))
	{
		void *pointer = (void *)(value & ~HEAP_MASK);
		struct alloc *alloc = pointer - sizeof(struct alloc);

		if (!pointer)
		{
			// TODO: Where are these coming from? Maybe this is a C
			// stack variable that we are interpreting as beign in
			// Lisp stack space on accident?
			fprintf(stderr, "lisp:gc:warning: value %x is a null pointer\n", value);
			return;
		}

		// Only recursively mark if this hasn't been marked yet. i.e. prevent
		// marking circular references twice
		if (alloc->mark != gc_mark)
		{
			++*marked;

			alloc->mark = gc_mark;

			// printf("[ GC ] val =");
			// printval(alloc_to_value(alloc), 2);

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
			fprintf(stderr, "[ GC ] freeing: %p\n", a);
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

	// For every stack frame until the base of the stack
	while (esp_p < gc_base)
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
