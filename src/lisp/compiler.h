#pragma once

#include "lisp.h"
#include <dasm_proto.h>
#include <stdbool.h>
#include <stdint.h>

struct function
{
	char *name;
	int nargs; // number of arguments

	union {
		value_t (*def0)();
		value_t (*def1)(value_t);
		value_t (*def2)(value_t, value_t);
		value_t (*def3)(value_t, value_t, value_t);
		void *code_ptr;
		uintptr_t code_addr;
	};

	struct function *prev;
};

struct environment
{
	struct function *first;
};

struct variable
{
	char *name;
	int number;
	struct variable *prev;
};

// local environment
struct local
{
	int num_vars;
	struct variable *first;
	int npc;
	int nextpc;
};

void compile_expression(struct environment *env, struct local *local,
                        value_t val, dasm_State **Dst);
void compile_expr_to_func(struct environment *env, char *name, value_t val,
                          dasm_State **Dst);
int nextpc(struct local *local, dasm_State **Dst);
// Compile top-level declaration
void compile_tl(value_t val, struct environment *env);
struct environment compile_all(struct istream *is);
struct function *find_function(struct environment *env, char *name);
