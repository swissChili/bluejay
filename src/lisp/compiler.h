#pragma once

#include "lisp.h"
#include <stdbool.h>
#include <stdint.h>
#include <dasm_proto.h>

struct function
{
	char *name;
	int nargs; // number of arguments

	union {
		value_t (*def0) ();
		value_t (*def1) (value_t);
		value_t (*def2) (value_t, value_t);
		value_t (*def3) (value_t, value_t, value_t);
		void *code_ptr;
		uintrptr_t code_addr;
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
	// temps are accessed at ebp - 8 * (num_vars + temp)
	bool temps[ 64 ];
	int num_vars;
	struct variable *first;
};

void compile_expression (struct environment *env, struct local *local,
                         value_t val, dasm_State **Dst);
void compile (struct istream *is);
struct function *find_function (struct environment *env, char *name);
