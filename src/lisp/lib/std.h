#pragma once

#include "../compiler.h"
#include "../lisp.h"

value_t l_plus(value_t a, value_t b);
value_t l_printval(value_t val);

void add_function(struct environment *env, char *name, void *func, struct args *args, enum namespace ns);
void add_c_function(struct environment *env, char *name, void *func, int nargs);
void load_std(struct environment *env);
bool load_library(struct environment *env, char *name);
