#include "compiler.h"
#include "gc.h"
#include "lisp.h"
#include "plat/plat.h"

int main(int argc, char **argv)
{
	gc_push_segment(NULL, 0);
	
	if (argc < 2)
	{
		puts("pass the program you want to run as the first argument please");
		return 1;
	}

	bool ok;
	struct environment *env = NULL;
	struct error compile_error;
	if (!IS_OKAY((compile_error = compile_file(argv[1], &env))))
	{
		ereport(compile_error);
		goto done;
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
