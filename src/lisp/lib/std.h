#pragma once

#include "../lisp.h"
#include "../compiler.h"

value_t l_plus (value_t a, value_t b);

void add_function (struct environment *env, char *name, void *func, int nargs);
void load_std (struct environment *env);
