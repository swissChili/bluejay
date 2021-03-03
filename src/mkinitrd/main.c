#include <initrd/initrd.h>
#include <stdio.h>
#include <string.h>

int file_size(FILE *file)
{
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	return size;
}

int main(int argc, char **argv)
{
	if (argc == 1 || (argc > 1 && !strcmp(argv[1], "-h")))
	{
		fprintf(stderr, "%s <output> [input...]\n", argv[0]);
		return argc == 1;
	}

	FILE *out; /* output file */

	if (!strcmp(argv[1], "-"))
		out = stdin;
	else
		out = fopen(argv[1], "w");

	if (!out)
	{
		fprintf(stderr, "Failed to open %s\n", argv[1]);
		return 1;
	}

	/* Header */
	int num_files = argc - 2;
	struct initrd_global_header header = {
		.magic = INITRD_MAGIC,
		.num_files = num_files,
	};

	fwrite(&header, sizeof(header), 1, out);

	for (int i = 0; i < num_files; i++)
	{
		FILE *in = fopen(argv[i + 2], "rb");
		if (!in)
		{
			fprintf(stderr, "Failed to open %s\n", argv[i + 2]);
			return 2;
		}

		struct initrd_file_header file = {
			.size = file_size(in),
		};

		strncpy(file.name, 64, argv[i + 2]);

		char c;
		while ((c = getc(in)) != EOF)
		{
			putc(c, out);
		}
		fclose(in);
	}
	fclose(out);
}
