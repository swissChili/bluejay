#pragma once

#include "../compiler.h"
#include "../lisp.h"

value_t l_plus(value_t a, value_t b);
value_t l_printval(value_t val);

void add_function(struct environment *env, char *name, void *func, int nargs, enum namespace ns);
void load_std(struct environment *env);
bool load_library(struct environment *env, char *name);
