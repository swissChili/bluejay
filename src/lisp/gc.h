#pragma once

#include "lisp.h"

// I hate this
extern value_t *gc_base;

void gc_set_base_here();

value_t alloc_to_value(struct alloc *a);
void _do_gc(unsigned int esp, unsigned int ebp);
void _mark(value_t value, unsigned int *marked);
void _sweep();
void free_all();
