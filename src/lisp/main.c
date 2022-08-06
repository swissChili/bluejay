#include "compiler.h"
#include "gc.h"
#include "lisp.h"
#include "plat/plat.h"
#include "lib/std.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
	gc_push_segment(NULL, 0);
	
	struct environment *env = NULL;
	struct error compile_error;

	if (argc < 2)
	{
		env = malloc(sizeof(struct environment));
		env->first = NULL;
		env->first_loaded = NULL;

		if (!IS_OKAY((compile_error = load_std(env))))
		{
			ereport(compile_error);
			goto done;
		}

		if (!load_library(env, "repl"))
			goto done;
	}
	else
	{
		if (!IS_OKAY((compile_error = compile_file(argv[1], &env))))
		{
			ereport(compile_error);
			goto done;
		}
	}

	struct function *lisp_main_f = find_function(env, "main");

	if (lisp_main_f)
	{
		value_t (*lisp_main)() = lisp_main_f->def0;
		gc_prepare_call(0);
		lisp_main();
	}
	else
	{
		fprintf(stderr, "No MAIN function defined! nothing to do\n");
	}

done:
	free_all();
	if (env)
		del_env(env);
	gc_pop_segment();
}
