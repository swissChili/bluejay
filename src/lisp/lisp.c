#include "lisp.h"
#include "plat/plat.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct alloc *first_a = NULL, *last_a = NULL;

value_t nil = 0b00101111; // magic ;)
value_t t = 1 << 3;

unsigned char max_pool = 0, current_pool = 0;

void err(const char *msg)
{
	fprintf(stderr, "ERROR: %s\n", msg);
	exit(1);
}

value_t intval(int i)
{
	i <<= 2;
	i |= INT_TAG;
	return i;
}

value_t cons(value_t car, value_t cdr)
{
	struct cons_alloc *item = malloc_aligned(sizeof(struct cons_alloc));
	struct cons *c = &item->cons;

	c->car = car;
	c->cdr = cdr;

	item->alloc.type_tag = CONS_TAG;
	item->alloc.pool = current_pool;

	if (last_a)
	{
		item->alloc.prev = last_a;
		last_a->next = item;
		item->alloc.next = NULL;
		last_a = item;
	}
	else
	{
		item->alloc.prev = item->alloc.next = NULL;
		first_a = last_a = item;
	}

	value_t v = (value_t)c;
	v |= CONS_TAG;

	return v;
}

void skipws(struct istream *is)
{
start:
	while (isspace(is->peek(is)))
		is->get(is);

	if (is->peek(is) == ';')
	{
		while (is->get(is) != '\n')
		{}

		// Only time I ever use labels is for stuff like this. Compiler would
		// probably optimize this if I used recursion but I don't want to
		// bother.
		goto start;
	}
}

bool isallowedchar(char c)
{
	return (c >= '#' && c <= '\'') || (c >= '*' && c <= '/') ||
	       (c >= '>' && c <= '@');
}

bool issymstart(char c)
{
	return isalpha(c) || isallowedchar(c);
}

bool issym(char c)
{
	return isalpha(c) || isallowedchar(c) || isdigit(c);
}

bool readsym(struct istream *is, value_t *val)
{
	skipws(is);

	if (!issymstart(is->peek(is)))
		return false;

	int size = 8;
	char *s = malloc_aligned(size);

	s[0] = is->get(is);

	for (int i = 1;; i++)
	{
		if (issym(is->peek(is)))
		{
			if (i >= size)
			{
				size *= 2;
				s = realloc_aligned(s, size);
			}

			s[i] = is->get(is);
		}
		else
		{
			s[i] = 0;
			*val = (value_t)s;
			*val |= SYMBOL_TAG;

			return true;
		}
	}
}

bool readstr(struct istream *is, value_t *val)
{
	skipws(is);

	if (is->peek(is) != '"')
		return false;

	bool escape = false;
	int size = 8;
	char *s = malloc_aligned(size);

	(void)is->get(is);

	for (int i = 0;; i++)
	{
		if (is->peek(is) != '"')
		{
			if (i >= size)
			{
				size *= 2;
				s = realloc_aligned(s, size);
			}

			char c = is->get(is);

			if (escape && c == 'n')
				c = '\n';
			else if (escape && c == '\\')
				c = '\\';

			if (c == '\\' && !escape)
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
			is->get(is);

			*val = (value_t)s;
			*val |= STRING_TAG;

			return true;
		}
	}
}

void printval(value_t v, int depth)
{
	for (int i = 0; i < depth; i++)
		printf("  ");

	if (symbolp(v))
	{
		printf("'%s\n", (char *)(v ^ SYMBOL_TAG));
	}
	else if (stringp(v))
	{
		printf("\"%s\"\n", (char *)(v ^ STRING_TAG));
	}
	else if (integerp(v))
	{
		printf("%d\n", v >> 2);
	}
	else if (consp(v))
	{
		if (listp(v))
		{
			printf("list:\n");

			for (value_t n = v; !nilp(n); n = cdr(n))
			{
				printval(car(n), depth + 1);
			}
		}
		else
		{
			printf("cons:\n");
			printval(car(v), depth + 1);
			printval(cdr(v), depth + 1);
		}
	}
	else if (nilp(v))
	{
		printf("nil\n");
	}
	else
	{
		printf("<unknown %d>\n", v);
	}
}

bool readlist(struct istream *is, value_t *val)
{
	skipws(is);

	if (is->peek(is) != '(')
		return false;

	is->get(is);

	*val = readn(is);

	if (is->peek(is) != ')')
	{
		is->showpos(is, stderr);
		err("Unterminated list");
		return false;
	}
	is->get(is);

	return true;
}

bool readint(struct istream *is, value_t *val)
{
	skipws(is);

	int number = 0;

	if (!isdigit(is->peek(is)))
		return false;

	while (isdigit(is->peek(is)))
	{
		number *= 10;
		number += is->get(is) - '0';
	}

	*val = intval(number);
	return true;
}

bool readquote(struct istream *is, value_t *val)
{
	skipws(is);

	char c = is->peek(is);

	if (c == '\'' || c == '`' || c == ',')
	{
		is->get(is);

		if (c == '`' && is->peek(is) == '@')
		{
			// This is actually a splice
			is->get(is);
			c = '@';
		}

		// Read the next form and wrap it in the appropriate function

		value_t wrapped;
		bool has_next = read1(is, &wrapped);

		if (!has_next)
		{
			fprintf(stderr, "Expected a form after reader macro char %c\n", c);
			is->showpos(is, stderr);
			err("Invalid reader macro");
			return false;
		}

		value_t symbol = nil;

		switch (c)
		{
		case '\'':
			symbol = symval("quote");
			break;
		case '`':
			symbol = symval("backquote");
			break;
		case ',':
			symbol = symval("unquote");
			break;
		case '@':
			symbol = symval("unquote-splice");
			break;
		}

		*val = cons(symbol, cons(wrapped, nil));

		return true;
	}
	else
	{
		return false;
	}
}

bool read1(struct istream *is, value_t *val)
{
	// This could all be one big short-circuiting || but that is ugly.
	if (readquote(is, val))
		return true;

	if (readsym(is, val))
		return true;

	if (readstr(is, val))
		return true;

	if (readint(is, val))
		return true;

	if (readlist(is, val))
		return true;

	return false;
}

value_t readn(struct istream *is)
{
	value_t first = nil;
	value_t *last = &first;

	value_t read_val;

	while (read1(is, &read_val))
	{
		*last = cons(read_val, nil);
		last = cdrref(*last);
	}

	return first;
}

bool startswith(struct istream *s, char *pattern)
{
	char *check = strdup(pattern);
	s->read(s, check, strlen(pattern));

	bool res = strcmp(check, pattern) == 0;
	free(check);

	return res;
}

value_t strval(char *str)
{
	value_t v;

	char *a = malloc_aligned(strlen(str) + 1);
	strcpy(a, str);
	v = (value_t)a;
	v |= STRING_TAG;

	return v;
}

value_t symval(char *str)
{
	value_t v;

	char *a = malloc_aligned(strlen(str) + 1);
	strcpy(a, str);
	v = (value_t)a;
	v |= SYMBOL_TAG;

	return v;
}

bool integerp(value_t v)
{
	return (v & INT_MASK) == INT_TAG;
}

bool symbolp(value_t v)
{
	return (v & HEAP_MASK) == SYMBOL_TAG;
}

bool stringp(value_t v)
{
	return (v & HEAP_MASK) == STRING_TAG;
}

bool consp(value_t v)
{
	return (v & HEAP_MASK) == CONS_TAG;
}

bool heapp(value_t v)
{
	return consp(v) || stringp(v) || symbolp(v);
}

bool listp(value_t v)
{
	value_t next = v;

	while (consp(next))
	{
		next = cdr(next);
	}

	return nilp(next);
}

value_t car(value_t v)
{
	if (!consp(v))
		return nil;

	return *carref(v);
}

value_t cdr(value_t v)
{
	if (!consp(v))
		return nil;

	return *cdrref(v);
}

value_t *carref(value_t v)
{
	if (!consp(v))
		return NULL;

	struct cons *c = (void *)(v ^ CONS_TAG);
	return &c->car;
}

value_t *cdrref(value_t v)
{
	if (!consp(v))
		return NULL;

	struct cons *c = (void *)(v ^ CONS_TAG);
	return &c->cdr;
}

bool nilp(value_t v)
{
	return v == nil;
}

int length(value_t v)
{
	int i = 0;

	for (; !nilp(v); v = cdr(v))
		i++;

	return i;
}

value_t elt(value_t v, int index)
{
	for (int i = 0; i < index; i++)
	{
		v = cdr(v);
	}

	return car(v);
}

bool symstreq(value_t sym, char *str)
{
	if ((sym & HEAP_MASK) != SYMBOL_TAG)
		return false;

	return strcmp((char *)(sym ^ SYMBOL_TAG), str) == 0;
}

unsigned char make_pool()
{
	return ++max_pool;
}

unsigned char push_pool(unsigned char pool)
{
	unsigned char old = current_pool;
	current_pool = pool;
	return old;
}

void pop_pool(unsigned char pool)
{
	current_pool = pool;
}

bool pool_alive(unsigned char pool)
{
	return pool != 0;
}
