#include "lisp.h"

int main (int argc, char **argv)
{
	if ( argc < 2 )
	{
		puts ("pass the string you want to parse as the first argument please");
		return 1;
	}

	struct istream *is = new_stristream_nt (argv[ 1 ]);
	struct value val;

	while ( read1 (is, &val) )
	{
		printval (val, 0);
	}

	del_stristream (is);
}
