#pragma once

#include "lisp.h"

// I hate this
extern value_t *gc_base;

void gc_set_base_here();

void _do_gc(unsigned int esp, unsigned int ebp);
void _mark(unsigned int value);
void _sweep();
