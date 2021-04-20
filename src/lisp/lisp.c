#include "lisp.h"
#include "plat/plat.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (a) > (b) ? (b) : (a)

struct alloc_list *first_a = NULL, *last_a = NULL;

value_t nil = 0b00101111; // magic ;)

void err (const char *msg)
{
	fprintf (stderr, "ERROR: %s\n", msg);
}

value_t intval (int i)
{
	i <<= 2;
	i |= INT_TAG;
	return i;
}

value_t cons (value_t car, value_t cdr)
{
	struct cons *c = malloc_aligned (sizeof (struct cons));

	c->car = car;
	c->cdr = cdr;

	struct alloc_list *item = malloc (sizeof (struct alloc_list));
	item->type = T_CONS;
	item->cons_val = c;

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

	value_t v = (value_t) c;
	v |= CONS_TAG;

	return v;
}

void skipws (struct istream *is)
{
	while ( isspace (is->peek (is)) )
		is->get (is);
}

bool isallowedchar (char c)
{
	return (c >= '#' && c <= '\'') || (c >= '*' && c <= '/') ||
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

bool readsym (struct istream *is, value_t *val)
{
	skipws (is);

	if ( !issymstart (is->peek (is)) )
		return false;

	int size = 8;
	char *s = malloc_aligned (size);

	s[ 0 ] = is->get (is);

	for ( int i = 1;; i++ )
	{
		if ( issym (is->peek (is)) )
		{
			if ( i >= size )
			{
				size *= 2;
				s = realloc_aligned (s, size);
			}

			s[ i ] = is->get (is);
		}
		else
		{
			s[ i ] = 0;
			*val = (value_t) s;
			*val |= SYMBOL_TAG;

			return true;
		}
	}
}

bool readstr (struct istream *is, value_t *val)
{
	skipws (is);

	if ( is->peek (is) != '"' )
		return false;

	bool escape = false;
	int size = 8;
	char *s = malloc_aligned (size);

	(void)is->get (is);

	for ( int i = 0;; i++ )
	{
		if ( is->peek (is) != '"' )
		{
			if ( i >= size )
			{
				size *= 2;
				s = realloc_aligned (s, size);
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
				s[ i ] = c;
			}
		}
		else
		{
			is->get (is);

			*val = (value_t) s;
			*val |= STRING_TAG;

			return true;
		}
	}
}

void printval (value_t v, int depth)
{
	for ( int i = 0; i < depth; i++ )
		printf ("  ");

	if ( symbolp (v) )
	{
		printf ("'%s\n", (char *) (v ^ SYMBOL_TAG));
	}
	else if ( stringp (v) )
	{
		printf ("\"%s\"\n", (char *) (v ^ STRING_TAG));
	}
	else if ( integerp (v) )
	{
		printf ("%d\n", v >> 2);
	}
	else if ( consp (v) )
	{
		if ( listp (v) )
		{
			printf ("list:\n");

			for ( value_t n = v; !nilp (n); n = cdr (n) )
			{
				printval (car (n), depth + 1);
			}
		}
		else
		{
			printf ("cons:\n");
			printval (car (v), depth + 1);
			printval (cdr (v), depth + 1);
		}
	}
	else if ( nilp (v) )
	{
		printf ("nil\n");
	}
	else
	{
		printf ("<unknown %d>\n", v);
	}
}

bool readlist (struct istream *is, value_t *val)
{
	skipws (is);

	if ( is->peek (is) != '(' )
		return false;

	is->get (is);

	*val = readn (is);

	if ( is->peek (is) != ')' )
	{
		is->showpos (is, stderr);
		err ("Unterminated list");
		return false;
	}
	is->get (is);

	return true;
}

bool readint (struct istream *is, value_t *val)
{
	int number = 0;

	if ( !isdigit (is->peek (is)) )
		return false;

	while ( isdigit (is->peek (is)) )
	{
		number *= 10;
		number += is->get (is) - '0';
	}

	*val = intval (number);
	return true;
}

bool read1 (struct istream *is, value_t *val)
{
	if ( readsym (is, val) )
		return true;

	if ( readstr (is, val) )
		return true;

	if ( readint (is, val) )
		return true;

	if ( readlist (is, val) )
		return true;

	return false;
}

value_t readn (struct istream *is)
{
	value_t first = nil;
	value_t *last = &first;

	value_t read_val;

	while ( read1 (is, &read_val) )
	{
		*last = cons (read_val, nil);
		last = cdrref (*last);
	}

	return first;
}

struct stristream_private
{
	char *val;
	int i;
	int length;
	int line;
	int fromleft;
	int linestart;
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
	{
		char c = p->val[ p->i++ ];

		p->fromleft++;

		if ( c == '\n' )
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

int stristream_read (struct istream *s, char *buffer, int size)
{
	struct stristream_private *p = s->data;

	int len = MIN (size, p->length - p->i);
	memcpy (buffer, p->val, len);
	return len;
}

void stristream_showpos (struct istream *s, FILE *out)
{
	struct stristream_private *p = s->data;

	fprintf (out, "line: %d, char %d\n", p->line, p->fromleft);

	int end = p->length;

	for ( int i = p->linestart; i < p->length; i++ )
	{
		if ( p->val[ i ] == '\n' )
		{
			end = i;
			break;
		}
	}

	fprintf (out, "  | %.*s\n", end - p->linestart, p->val + p->linestart);
	fprintf (out, "  | ");
	for ( int i = 0; i < p->fromleft - 1; i++ )
		fprintf (out, " ");

	fprintf (out, "\033[31m^\033[0m\n");
}

struct istream *new_stristream (char *str, int length)
{
	struct istream *is = malloc (sizeof (struct istream));
	struct stristream_private *p = malloc (sizeof (struct stristream_private));

	p->val = strndup (str, length);
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

	return is;
}

void del_stristream (struct istream *stristream)
{
	struct stristream_private *p = stristream->data;
	free (p->val);
	free (p);
	free (stristream);
}

struct istream *new_stristream_nt (char *str)
{
	return new_stristream (str, strlen (str));
}

bool startswith (struct istream *s, char *pattern)
{
	char *check = strdup (pattern);
	s->read (s, check, strlen (pattern));

	bool res = strcmp (check, pattern) == 0;
	free (check);

	return res;
}

value_t strval (char *str)
{
	value_t v;

	char *a = malloc_aligned (strlen (str) + 1);
	v = (value_t) a;
	v |= STRING_TAG;

	return v;
}

bool integerp (value_t v)
{
	return (v & INT_MASK) == INT_TAG;
}

bool symbolp (value_t v)
{
	return (v & HEAP_MASK) == SYMBOL_TAG;
}

bool stringp (value_t v)
{
	return (v & HEAP_MASK) == STRING_TAG;
}

bool consp (value_t v)
{
	return (v & HEAP_MASK) == CONS_TAG;
}

bool listp (value_t v)
{
	value_t next = v;

	while ( consp (next) )
	{
		next = cdr (next);
	}

	return nilp (next);
}

value_t car (value_t v)
{
	if ( !consp (v) )
		return nil;

	return *carref (v);
}

value_t cdr (value_t v)
{
	if ( !consp (v) )
		return nil;

	return *cdrref (v);
}

value_t *carref (value_t v)
{
	if ( !consp (v) )
		return NULL;

	struct cons *c = (void *) (v ^ CONS_TAG);
	return &c->car;
}

value_t *cdrref (value_t v)
{
	if ( !consp (v) )
		return NULL;

	struct cons *c = (void *) (v ^ CONS_TAG);
	return &c->cdr;
}

bool nilp (value_t v)
{
	return v == nil;
}

int length (value_t v)
{
	int i = 0;

	FOREACH (item, v)
	{
		(void) item;
		i++;
	}

	return i;
}
