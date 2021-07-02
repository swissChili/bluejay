#include "istream.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct stristream_private
{
	char *val;
	int i;
	int length;
	int line;
	int fromleft;
	int linestart;
};

int stristream_peek(struct istream *is)
{
	struct stristream_private *p = is->data;

	if (p->i < p->length)
		return p->val[p->i];
	else
		return -1;
}

int stristream_get(struct istream *is)
{
	struct stristream_private *p = is->data;

	if (p->i < p->length)
	{
		char c = p->val[p->i++];

		p->fromleft++;

		if (c == '\n')
		{
			p->fromleft = 1;
			p->line++;
			p->linestart = p->i;
		}

		return c;
	}
	else
		return -1;
}

int stristream_read(struct istream *s, char *buffer, int size)
{
	struct stristream_private *p = s->data;

	int len = MIN(size, p->length - p->i);
	memcpy(buffer, p->val, len);
	return len;
}

void stristream_showpos(struct istream *s, FILE *out)
{
	struct stristream_private *p = s->data;

	fprintf(out, "line: %d, char %d\n", p->line, p->fromleft);

	int end = p->length;

	for (int i = p->linestart; i < p->length; i++)
	{
		if (p->val[i] == '\n')
		{
			end = i;
			break;
		}
	}

	fprintf(out, "  | %.*s\n", end - p->linestart, p->val + p->linestart);
	fprintf(out, "  | ");
	for (int i = 0; i < p->fromleft - 1; i++)
		fprintf(out, " ");

	fprintf(out, "\033[31m^\033[0m\n");
}

void stristream_getpos(struct istream *is, int *line, char **name)
{
	struct stristream_private *p = is->data;

	*name = "<input literal>";
	*line = p->line;
}

struct istream *new_stristream(char *str, int length)
{
	struct istream *is = malloc(sizeof(struct istream));
	struct stristream_private *p = malloc(sizeof(struct stristream_private));

	p->val = strndup(str, length);
	p->i = 0;
	p->length = length;
	p->line = 1;
	p->fromleft = 1;
	p->linestart = 0;

	is->data = p;
	is->get = stristream_get;
	is->peek = stristream_peek;
	is->read = stristream_read;
	is->showpos = stristream_showpos;
	is->getpos = stristream_getpos;

	return is;
}

void del_stristream(struct istream *stristream)
{
	struct stristream_private *p = stristream->data;
	free(p->val);
	free(p);
	free(stristream);
}

struct istream *new_stristream_nt(char *str)
{
	return new_stristream(str, strlen(str));
}

struct fistream_private
{
	FILE *file;
	int next;
	bool has_next;
	int line;
};

int fistream_peek(struct istream *is)
{
	struct fistream_private *p = is->data;

	if (p->has_next)
		return p->next;

	p->next = fgetc(p->file);
	p->has_next = true;
	return p->next;
}

int fistream_get(struct istream *is)
{
	struct fistream_private *p = is->data;

	char c;

	if (p->has_next)
	{
		p->has_next = false;
		c = p->next;
	}
	else
		c = fgetc(p->file);

	if (c == '\n')
		p->line++;
	
	return c;
}

int fistream_read(struct istream *is, char *buffer, int size)
{
	struct fistream_private *p = is->data;

	int offset = 0;

	if (p->has_next)
	{
		*buffer = p->next;
		p->has_next = false;
		buffer++;
		size--;
		offset = 1;
	}

	return (int)fread(buffer, 1, size, p->file) + offset;
}

void fistream_showpos(struct istream *s, FILE *out)
{
	// TODO: implement
}

void fistream_getpos(struct istream *is, int *line, char **name)
{
	struct fistream_private *p = is->data;

	*line = p->line;
	*name = "<FILE *>";
}

struct istream *new_fistream(char *path, bool binary)
{
	struct istream *is = malloc(sizeof(struct istream));

	FILE *fp = fopen(path, binary ? "rb" : "r");

	if (fp == NULL)
	{
		free(is);
		return NULL;
	}

	struct fistream_private *p = is->data =
	    malloc(sizeof(struct fistream_private));

	p->has_next = false;
	p->file = fp;
	p->line = 1;

	is->data = p;
	is->get = fistream_get;
	is->peek = fistream_peek;
	is->read = fistream_read;
	is->showpos = fistream_showpos;
	is->getpos = fistream_getpos;

	return is;
}

void del_fistream(struct istream *is)
{
	struct fistream_private *p = is->data;

	fclose(p->file);

	free(is->data);
	free(is);
}
