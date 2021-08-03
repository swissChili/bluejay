#pragma once

#include <stdbool.h>
#include <stdio.h>

#define MIN(a, b) (a) > (b) ? (b) : (a)

/// Virtual class representing an input stream. Subclasses must implement every
/// function.
struct istream
{
	void *data;

	/// Returns -1 on error
	int (*peek)(struct istream *s);
	/// Returns -1 on error
	int (*get)(struct istream *s);

	int (*read)(struct istream *s, char *buffer, int size);

	void (*showpos)(struct istream *s, FILE *out);

	void (*getpos)(struct istream *s, int *line, char **name);
};

struct istream *new_stristream(char *str, int length);
// same as above but null terminated
struct istream *new_stristream_nt(char *str);
void del_stristream(struct istream *stristream);

struct istream *new_fistream(char *path, bool binary);
void del_fistream(struct istream *fistream);
