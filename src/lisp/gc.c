#include "gc.h"
#include "lisp.h"

void _mark(unsigned int value)
{

}

void _sweep()
{

}

void _do_gc(unsigned int esp, unsigned int ebp)
{
	unsigned int *esp_p = (unsigned int *)esp,
		*ebp_p = (unsigned int *)ebp;
	
	for (int i = 0; esp_p + i < ebp_p; i++)
	{
		_mark(esp_p[i]);
	}
}
