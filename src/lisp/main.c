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
	value_t val;

	while ( read1 (is, &val) )
	{
//		printval (val, 0);
		compile (val);
	}

	/* printf ("COMPILING\n"); */

	return 0;

	del_stristream (is);
}
