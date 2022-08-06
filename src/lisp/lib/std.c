#include "std.h"
#include "classes.h"
#include "../gc.h"
#include "../compiler.h"
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

void add_c_varargs(struct environment *env, char *name, void *func, int nargs)
{
	struct args *args = new_args();
	args->num_required = nargs;
	args->variadic = true;

	add_function(env, name, func, args, NS_FUNCTION);
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
	struct error err = { 0 };
	
	if (!IS_OKAY((err = read1(is, &val))))
	{
		ereport(err);

		del_stristream(is);
		free(string);
		// tail recursion, yay!
		return l_read_stdin();
	}

	del_stristream(is);
	free(string);

	return val;
}

value_t l_num_eq(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
	{
		return nil;
	}

	return (a >> 2) == (b >> 2) ? t : nil;
}

value_t l_num_gt(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (a >> 2) > (b >> 2) ? t : nil;
}

value_t l_num_lt(value_t a, value_t b)
{
	if (!integerp(a) || !integerp(b))
		return nil;

	return (a >> 2) < (b >> 2) ? t : nil;
}

value_t l_append(value_t l)
{
	if (nilp(l))
		return l;

	value_t first = nil;
	value_t *last = NULL;

	for (value_t item = l; !nilp(item); item = cdr(item))
	{
		value_t a = car(item);

		if (!listp(a))
		{
			value_t new = cons(a, nil);
			*last = new;
			last = cdrref(new);
			continue;
		}

		for (value_t i = a; !nilp(i); i = cdr(i))
		{
			value_t b = car(i);

			if (!last)
			{
				first = cons(b, nil);
				last = cdrref(first);
			}
			else
			{
				value_t new = cons(b, nil);
				*last = new;
				last = cdrref(new);
			}
		}
	}

	return first;
}

value_t l_gc_stats()
{
	struct gc_stats stats = gc_get_stats();

	return cons(intval(stats.total_allocs),
				cons(intval(stats.gc_runs),
					 nil));
}

value_t l_list_functions(value_t env)
{
	if (!integerp(env))
		return nil;

	struct environment *e = (void *)env;
	value_t list = nil;

	for (struct function *fun = e->first; fun; fun = fun->prev)
	{
		list = cons(symval(fun->name), list);
	}

	return list;
}

value_t l_string_to_symbol(value_t string)
{
	if (!stringp(string))
		return nil;

	return symval((char *)(string ^ STRING_TAG));
}

value_t l_symbol_to_string(value_t string)
{
	if (!symbolp(string))
		return nil;

	return strval((char *)(string ^ SYMBOL_TAG));
}

value_t l_string_length(value_t string)
{
	if (!stringp(string))
		return intval(0);

	return intval(strlen((char *)(string ^ STRING_TAG)));
}

value_t l_concat(value_t strings)
{
	struct alloc *string_alloc = malloc_aligned(sizeof(struct alloc));
	int lengths = 0;

	for (value_t str = strings; !nilp(str); str = cdr(str))
	{
		if (!stringp(car(str)))
			continue;

		int len = strlen((char *)(car(str) ^ STRING_TAG));
		string_alloc = realloc_aligned(string_alloc,
									   sizeof(struct alloc) + lengths + len);

		memcpy((void *)string_alloc + sizeof(struct alloc) + lengths,
			   (char *)(car(str) ^ STRING_TAG),
			   len);

		lengths += len;
	}

	add_this_alloc(string_alloc, STRING_TAG);

	return (value_t)(string_alloc + 1) | STRING_TAG;
}

value_t l_gensym()
{
	static int symcnt = 0;
	char buffer[32] = {0};
	snprintf(buffer, 32, "-sym-%d", symcnt++);
	return symval(buffer);
}

#define LISP_PREDICATE(name) value_t l_##name(value_t v) { return name(v) ? t : nil; }

LISP_PREDICATE(listp)
LISP_PREDICATE(integerp)
LISP_PREDICATE(symbolp)
LISP_PREDICATE(closurep)
LISP_PREDICATE(consp)
LISP_PREDICATE(classp)

#undef LISP_PREDICATE

struct error load_std(struct environment *env)
{
	E_INIT();

	add_c_function(env, "+", l_plus, 2);
	add_c_function(env, "-", l_minus, 2);
	add_c_function(env, "*", l_times, 2);
	add_c_function(env, "/", l_divide, 2);
	add_c_function(env, "=", l_num_eq, 2);
	add_c_function(env, "<", l_num_lt, 2);
	add_c_function(env, ">", l_num_gt, 2);

	add_c_function(env, "car", car, 1);
	add_c_function(env, "cdr", cdr, 1);
	add_c_function(env, "cons", cons, 2);

	add_c_function(env, "print", l_printval, 1);
	add_c_function(env, "read-stdin", l_read_stdin, 0);
	add_c_function(env, "apply", l_apply, 2);
	add_c_varargs(env,  "append", l_append, 0);

	add_c_function(env, "nilp", l_nilp, 1);
	add_c_function(env, "listp", l_listp, 1);
	add_c_function(env, "integerp", l_integerp, 1);
	add_c_function(env, "symbolp", l_symbolp, 1);
	add_c_function(env, "closurep", l_closurep, 1);
	add_c_function(env, "functionp", l_closurep, 1);
	add_c_function(env, "consp", l_consp, 1);
	
	add_c_function(env, "elt", l_elt, 2);

	add_c_function(env, "gc-stats", l_gc_stats, 0);
	add_c_function(env, "env-functions", l_list_functions, 1);

	add_c_varargs(env, "concat", l_concat, 0);
	add_c_function(env, "string-length", l_string_length, 1);
	add_c_function(env, "string->symbol", l_string_to_symbol, 1);
	add_c_function(env, "symbol->string", l_symbol_to_string, 1);
	add_c_function(env, "gensym", l_gensym, 0);

	load_classes(env);

	if (!load_library(env, "std"))
	{
		fprintf(stderr, "Not found std\n");
		THROW(ENOTFOUND, "Could not load library `std`, make sure your $LISP_LIBRARY_PATH is correct.");
	}

	OKAY();
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
			// fprintf(stderr, "loading path: %s\n", path);
			return load(env, path);
		}

		snprintf(path, 512, "%s/%s/%s.lisp", p, name, name);

		if (file_exists(path))
		{
			// fprintf(stderr, "loading path: %s\n", path);
			return load(env, path);
		}
	}

	return false;
}
