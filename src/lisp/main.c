#include "compiler.h"
#include "lisp.h"
#include "gc.h"
#include "plat/plat.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		puts("pass the program you want to run as the first argument please");
		return 1;
	}

	bool ok;
	struct environment *env = compile_file(argv[1], &ok);

	if (!ok)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		return 1;
	}

	value_t (*lisp_main)() = find_function(env, "main")->def0;

	if (lisp_main)
	{
		gc_set_base_here();
		lisp_main();
	}
	else
	{
		fprintf(stderr, "No MAIN function defined! nothing to do\n");
	}

	free_all();
	del_env(env);
}
