#include "std.h"
#include <stdlib.h>

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

void add_function(struct environment *env, char *name, void *func, int nargs, enum namespace ns)
{
	struct function *last, *new = malloc(sizeof(struct function));

	last = env->first;
	new->prev = last;
	new->name = name;
	new->nargs = nargs;
	new->code_ptr = func;
	new->namespace = ns;

	env->first = new;
}

void load_std(struct environment *env)
{
	add_function(env, "+", l_plus, 2, NS_FUNCTION);
	add_function(env, "-", l_minus, 2, NS_FUNCTION);
	add_function(env, "*", l_times, 2, NS_FUNCTION);
	add_function(env, "/", l_divide, 2, NS_FUNCTION);

	add_function(env, "car", car, 1, NS_FUNCTION);
	add_function(env, "cdr", cdr, 1, NS_FUNCTION);
	add_function(env, "cons", cons, 2, NS_FUNCTION);

	add_function(env, "print", l_printval, 1, NS_FUNCTION);

	add_function(env, "apply", l_apply, 2, NS_FUNCTION);
}
