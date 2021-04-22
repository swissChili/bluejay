#include "lisp.h"
#include "compiler.h"

int main (int argc, char **argv)
{
	if ( argc < 2 )
	{
		puts ("pass the string you want to parse as the first argument please");
		return 1;
	}

	struct istream *is = new_stristream_nt (argv[ 1 ]);

	struct environment env = compile_all (is);
	value_t (*lisp_main) () = find_function(&env, "main")->def0;
	lisp_main ();

//	del_stristream (is);
}
