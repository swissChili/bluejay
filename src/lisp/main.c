#include "compiler.h"
#include "lisp.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		puts("pass the string you want to parse as the first argument please");
		return 1;
	}

	struct istream *is = new_fistream(argv[1], false);

	if (is == NULL)
	{
		fprintf(stderr, "Could not open %s\n", argv[1]);
		return 1;
	}

<<<<<<< HEAD
	struct environment env = compile_all(is);
	value_t (*lisp_main)() = find_function(&env, "main")->def0;
	lisp_main();

	del_fistream(is);
=======
	struct environment env = compile_all (is);
	value_t (*lisp_main) () = find_function(&env, "main")->def0;
	lisp_main ();
>>>>>>> 6afa62d (Remove free)
}
