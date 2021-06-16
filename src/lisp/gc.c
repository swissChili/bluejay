#include "gc.h"
#include "lisp.h"

value_t *gc_base;

void __attribute__((noinline)) gc_set_base_here()
{
	// Move the current stack top address to gc_base. This works nicely because
	// it means that when another (presumably lisp) function is called right
	// after this, the stack top for it will be the same as for this function.
	asm("movl %%esp, %0" : "=g"(gc_base));
}

void _mark(value_t value)
{
	if (heapp(value))
	{

	}
}

void _sweep()
{

}

void _do_gc(unsigned int esp, unsigned int ebp)
{
	value_t *esp_p = (value_t *)esp,
		*ebp_p = (value_t *)ebp;
	
	int num_marked = 0;

	// For every stack frame until the base of the stack
	while (esp_p < gc_base)
	{
		// Walk up the stack until we reach either the frame pointer or the base
		// of the stack. Basically walk to the top of this function's stack
		// frame.
		for (; esp_p < ebp_p && esp_p < gc_base; esp_p++)
		{
			_mark(*esp_p);
			num_marked++;
		}

		// Set the frame pointer to the frame pointer on the stack
		ebp_p = (value_t *)*esp_p;

		// Step up two stack slots, one for the frame pointer and one for the
		// return address.
		esp_p += 2;
	}

	fprintf(stderr, "Marked %d\n", num_marked);
}
