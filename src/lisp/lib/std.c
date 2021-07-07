#include "std.h"
#include "../plat/plat.h"
#include <stdlib.h>
#include <string.h>

value_t l_plus(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (((a >> 2) + (b >> 2)) << 2) | INT_TAG;
}

value_t l_minus(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (((a >> 2) - (b >> 2)) << 2) | INT_TAG;
}

value_t l_times(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (((a >> 2) * (b >> 2)) << 2) | INT_TAG;
}

value_t l_divide(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (((a >> 2) / (b >> 2)) << 2) | INT_TAG;
}

value_t l_printval(value_t val)
{
	printval(val, 0);
	return nil;
}

value_t l_apply(value_t func, value_t args)
{
	if (!closurep(func))
		return nil;

	if (!listp(args))
		return nil;

	return call_list_closure((struct closure *)(func ^ CLOSURE_TAG), args);
}

value_t l_nilp(value_t val)
{
	return nilp(val) ? t : nil;
}

void add_function(struct environment *env, char *name, void *func, struct args *args, enum namespace ns)
{
	struct function *last, *new = malloc(sizeof(struct function));

	last = env->first;
	new->prev = last;
	new->name = name;
	new->args = args;
	new->code_ptr = func;
	new->namespace = ns;

	env->first = new;
}

void add_c_function(struct environment *env, char *name, void *func, int nargs)
{
	struct args *args = new_args();
	args->num_required = nargs;

	add_function(env, name, func, args, NS_FUNCTION);
}

value_t l_elt(value_t seq, value_t i)
{
	if (!listp(seq) || !integerp(i))
		return nil;
	
	return elt(seq, i >> 2);
}

value_t l_read_stdin()
{
	char *string = read_input_line("lisp> ");
	if (!string)
		return nil;

	struct istream *is = new_stristream_nt(string);

	value_t val = nil;
	read1(is, &val);

	del_stristream(is);
	free(string);

	return val;
}

void load_std(struct environment *env)
{
	add_c_function(env, "+", l_plus, 2);
	add_c_function(env, "-", l_minus, 2);
	add_c_function(env, "*", l_times, 2);
	add_c_function(env, "/", l_divide, 2);

	add_c_function(env, "car", car, 1);
	add_c_function(env, "cdr", cdr, 1);
	add_c_function(env, "cons", cons, 2);

	add_c_function(env, "print", l_printval, 1);
	add_c_function(env, "read-stdin", l_read_stdin, 0);
	add_c_function(env, "apply", l_apply, 2);

	add_c_function(env, "nilp", l_nilp, 1);
	add_c_function(env, "elt", l_elt, 2);

	if (!load_library(env, "std"))
	{
		err("Could not load library `std`, make sure your $LISP_LIBRARY_PATH is correct.");
	}
}

bool load_library(struct environment *env, char *name)
{
	char *lib_paths = getenv("LISP_LIBRARY_PATH");

	if (!lib_paths)
		lib_paths = "/lib/lisp";

	for (char *p = strtok(lib_paths, ":"); p; p = strtok(NULL, ":"))
	{
		static char path[512];
		snprintf(path, 512, "%s/%s.lisp", p, name);

		if (file_exists(path))
		{
			return load(env, path);
		}

		snprintf(path, 512, "%s/%s/%s.lisp", p, name, name);

		if (file_exists(path))
		{
			return load(env, path);
		}
	}

	return false;
}
