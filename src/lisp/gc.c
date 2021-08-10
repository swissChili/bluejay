#include "gc.h"
#include "lisp.h"
#include "plat/plat.h"
#include <stdarg.h>
#include <string.h>

value_t *gc_base;
THREAD_LOCAL static unsigned int gc_mark;

struct gc_skipped_segment gc_segments[64] = { 0 };
int gc_current_segment = 0;

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
		if (pool_alive(a->pool) || a->mark == gc_mark)
		{
			// Marked or in living pool
			a = a->next;
		}
		else
		{
			// Free and remove from allocation list
			struct alloc *p = a->prev, *n = a->next;
			free_aligned(a);

			a = n;

			if (p)
				p->next = n;

			if (n)
				n->prev = p;
		}
	}
}

void _do_gc(unsigned int esp, unsigned int ebp)
{
	value_t *esp_p = (value_t *)esp,
		*ebp_p = (value_t *)ebp;
	
	unsigned int num_marked = 0;

	gc_mark++;

	for (int i = gc_current_segment; i <= 0; i--)
	{
		value_t *base = gc_segments[i].top;

		// For every stack frame until the base of the stack
		while (esp_p < base)
		{
			// Walk up the stack until we reach either the frame pointer or the base
			// of the stack. Basically walk to the top of this function's stack
			// frame.
			for (; esp_p < ebp_p && esp_p < base; esp_p++)
			{
				_mark(*esp_p, &num_marked);
			}

			// Set the frame pointer to the frame pointer on the stack
			ebp_p = (value_t *)*esp_p;

			// Step up two stack slots, one for the frame pointer and one for the
			// return address.
			esp_p += 2;
		}

		// Mark the values marked by the skipped segment
		for (int j = 0; j < gc_segments[i].num_marked; j++)
		{
			_mark(gc_segments[i].mark[j], &num_marked);
		}
		
		esp_p = gc_segments[i].bottom;
	}

	_sweep();
}

void free_all()
{
	for (struct alloc *a = first_a; a;)
	{
		struct alloc *next = a->next;
		free_aligned(a);
		a = next;
//		fprintf(stderr, "a = %p\n", a);
	}
}

void gc_skip(void *last_arg, ...)
{
	va_list list;
	va_start(list, last_arg);

	struct gc_skipped_segment *seg = &gc_segments[++gc_current_segment];
	memset(seg, 0, sizeof(struct gc_skipped_segment));

	seg->bottom = last_arg + sizeof(value_t);

	for (int i = 0; i < 8; i++)
	{
		value_t v = va_arg(list, value_t);
		if (nilp(v))
			break;
		seg->mark[i] = v;
		seg->num_marked++;
	}

	va_end(list);
}

void gc_resume()
{
	gc_resumen(0);
}

void gc_resumen(int n)
{
	void *esp;
	asm("movl %%esp, %0" : "=g"(esp));

	gc_segments[gc_current_segment].top = esp - sizeof(value_t) * n;;
}

void gc_endskip()
{
	gc_current_segment--;
}
