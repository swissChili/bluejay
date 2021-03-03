#include <initrd/initrd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	if (argc == 1 || (argc > 1 && !strcmp(argv[1], "-h")))
	{
		printf("%s <output> [input...]\n", argv[0]);
		return argc == 1;
	}

	FILE *out = fopen(argv[1], "w");
}
