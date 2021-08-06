#include "error.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

char *ehsprintf(const char *msg, ...)
{
	char *buf = malloc(1024);
	va_list list;
	va_start(list, msg);
	vsnprintf(buf, 1023, msg, list);
	va_end(list);

	return buf;
}

void ereport(struct error err)
{
	if (err.loc.file && err.loc.line)
		fprintf(stderr, "\033[31merror at\033[0m %s:%d\n", err.loc.file, err.loc.line);
	else
		fprintf(stderr, "\033[31merror\033[0m\n");
	
	if (err.message)
		fprintf(stderr, "%s\n", err.message);
	else
	{
		switch (err.code)
		{
		case EEXPECTED:
			fprintf(stderr, "Expected something but it was not found.\n");
			break;
		case EINVALID:
			fprintf(stderr, "Invalid input.\n");
			break;
		case ENOTFOUND:
			fprintf(stderr, "External resource not found.\n");
			break;
		default:
			fprintf(stderr, "Unknown error %d\n", err.code);
		}
	}
}
