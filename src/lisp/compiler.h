#pragma once

#include "lisp.h"
#include <stdbool.h>

struct function
{
	char *name;
	int nargs; // number of arguments

	union {
		struct value (*def0) ();
		struct value (*def1) (struct value);
		struct value (*def2) (struct value, struct value);
		struct value (*def3) (struct value, struct value, struct value);
		void *code_ptr;
		unsigned long code_addr;
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

void compile (struct istream *is);
struct function *find_function (struct environment *env, char *name);
