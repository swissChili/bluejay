#include "std.h"

value_t l_plus (value_t a, value_t b)
{
	if ( !integerp (a) || !integerp (b) )
		return nil;

	return (((a >> 2) + (b >> 2)) << 2) | INT_TAG;
}

void add_function (struct environment *env, char *name, void *func, int nargs)
{
	struct function *last,
		*new = malloc (sizeof (struct function));

	last = env->first;
	new->prev = last;
	new->name = name;
	new->nargs = nargs;
	new->code_ptr = func;

	env->prev = new;
}

void load_std (struct environment *env)
{
	add_function (env, "+", l_plus, 2);
}
