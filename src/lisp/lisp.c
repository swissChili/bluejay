#include "lisp.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define MIN(a, b) (a)>(b)?(b):(a)

struct alloc_list *first_a = NULL, *last_a = NULL;

struct value cons (struct value car, struct value cdr)
{
	struct cons *c = malloc (sizeof (struct cons));

	struct alloc_list *item = malloc (sizeof (struct alloc_list));
	item->type = T_CONS;
	item->data.cons_val = c;

	if ( last_a )
	{
		item->prev = last_a;
		last_a->next = item;
		item->next = NULL;
	}
	else
	{
		item->prev = item->next = NULL;
		first_a = last_a = item;
	}

	struct value v;
	v.tag.type = T_CONS;
	v.value.cons_val = c;

	return v;
}

void skipws (struct istream *is)
{
	while ( isspace (is->peek (is)) )
		is->get (is);
}

bool isallowedchar (char c)
{
	return (c >= '#' && c <= '\'') ||
		(c >= '*' && c <= '/') ||
		(c >= '>' && c <= '@');
}

bool issymstart (char c)
{
	return isalpha (c) || isallowedchar (c);
}

bool issym (char c)
{
	return isalpha (c) || isallowedchar (c) || isdigit (c);
}

bool readsym (struct istream *is, struct value *val)
{
	skipws (is);

	if ( !issymstart (is->peek (is)) )
		return false;

	int size = 8;
	char *s = malloc (size);

	s[0] = is->get (is);

	for ( int i = 1; ; i++ )
	{
		if ( issym (is->peek (is)) )
		{
			if ( i >= size )
			{
				size *= 2;
				s = realloc (s, size);
			}

			s[i] = is->get (is);
		}
		else
		{
			s[i] = 0;
			val->tag.type = T_SYMBOL;
			val->tag.length = i - 1;
			val->value.symbol_val = s;

			return true;
		}
	}
}

bool readstr (struct istream *is, struct value *val)
{
	skipws (is);

	if ( is->peek (is) != '"' )
		return false;

	bool escape = false;
	int size = 8;
	char *s = malloc (size);

	(void) is->get (is);

	for ( int i = 0; ; i++ )
	{
		if ( is->peek (is) != '"' )
		{
			if ( i >= size )
			{
				i *= 2;
				s = realloc (s, i);
			}
				
			char c = is->get (is);

			if ( escape && c == 'n' )
				c = '\n';
			else if ( escape && c == '\\' )
				c = '\\';

			if ( c == '\\' && !escape )
			{
				escape = true;
				i--; // will be incremented again, UGLY.
			}
			else
			{
				escape = false;
				s[i] = c;
			}
		}
		else
		{
			is->get (is);

			val->tag.type = T_STRING;
			val->tag.length = i;
			val->value.string_val = s;

			return true;
		}
	}
}

void printval (struct value v, int depth)
{
	for ( int i = 0; i < depth; i++ )
		printf("  ");

	switch (v.tag.type)
	{
	case T_SYMBOL:
		printf ("'%s\n", v.value.symbol_val);
		return;
	case T_STRING:
		printf ("\"%s\"\n", v.value.string_val);
		return;
	default:
		printf ("<unknown %d>\n", v.tag.type);
	}
}

bool read1 (struct istream *is, struct value *val)
{
	if ( readsym (is, val) )
		return true;

	if ( readstr (is, val) )
		return true;

	return false;
}

struct stristream_private
{
	char *val;
	int i;
	int length;
};

int stristream_peek (struct istream *is)
{
	struct stristream_private *p = is->data;

	if ( p->i < p->length )
		return p->val[ p->i ];
	else
		return -1;
}

int stristream_get (struct istream *is)
{
	struct stristream_private *p = is->data;

	if ( p->i < p->length )
		return p->val[ p->i++ ];
	else
		return -1;

}

int stristream_read (struct istream *s, char *buffer, int size)
{
	struct stristream_private *p = s->data;

	int len = MIN (size, p->length - p->i);
	memcpy (buffer, p->val, len);
	return len;
}

struct istream *new_stristream (char *str, int length)
{
	struct istream *is = malloc (sizeof (struct istream));
	struct stristream_private *p = malloc (sizeof (struct stristream_private));

	p->val = strndup (str, length);
	p->i = 0;
	p->length = length;

	is->data = p;
	is->get = stristream_get;
	is->peek = stristream_peek;
	is->read = stristream_read;

	return is;
}

void del_stristream(struct istream *stristream)
{
	struct stristream_private *p = stristream->data;
	free (p->val);
	free (p);
	free (stristream);
}

struct istream *new_stristream_nt (char *str)
{
	return new_stristream(str, strlen(str));
}

bool startswith (struct istream *s, const char *pattern)
{
	const char *check = strdup (pattern);
	s->read (s, check, strlen (pattern));

	bool res = strcmp (check, pattern) == 0;
	free (check);

	return res;
}
