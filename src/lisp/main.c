#include "lisp.h"

int main (int argc, char **argv)
{
	struct istream *is = new_stristream_nt ("abcde \"asdf dsf sdf\"");
	struct value val;

	while ( read1 (is, &val) )
	{
		printval (val, 0);
	}

	del_stristream (is);
}
