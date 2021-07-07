#include "lisp.h"
#include "plat/plat.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct alloc *first_a = NULL, *last_a = NULL;

value_t nil = 0b00101111; // magic ;)
value_t t = 1 << 3;

unsigned char max_pool = 0, current_pool = 0;

__attribute__((noreturn)) void err(const char *msg)
{
	fprintf(stderr, "ERROR: %s\n", msg);
	exit(1);
}

__attribute__((noreturn)) void err_at(value_t form, const char *msg, ...)
{
	int line = cons_line(form);
	char *file = cons_file(form);

	fprintf(stderr, "\033[31merror at\033[0m %s:%d\n", file, line);

	va_list list;
	va_start(list, msg);
	vfprintf(stderr, msg, list);
	va_end(list);
	fprintf(stderr, "\n");

	exit(1);
}

value_t intval(int i)
{
	i <<= 2;
	i |= INT_TAG;
	return i;
}

void add_this_alloc(struct alloc *a, int tag)
{
	a->type_tag = tag;
	a->pool = current_pool;

	if (last_a)
	{
		a->prev = last_a;
		last_a->next = a;
		a->next = NULL;
		last_a = a;
	}
	else
	{
		a->prev = a->next = NULL;
		first_a = last_a = a;
	}
}

value_t cons(value_t car, value_t cdr)
{
	struct cons_alloc *item = malloc_aligned(sizeof(struct cons_alloc));
	struct cons *c = &item->cons;

	c->car = car;
	c->cdr = cdr;
	c->line = 0;
	c->name = NULL;

	value_t v = (value_t)c;
	v |= CONS_TAG;

	add_this_alloc(&item->alloc, CONS_TAG);

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
		{
		}

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
	struct alloc *a = malloc_aligned(size + sizeof(struct alloc));
	add_this_alloc(a, SYMBOL_TAG);

	char *s = (char *)(a + 1);

	s[0] = is->get(is);

	for (int i = 1;; i++)
	{
		if (issym(is->peek(is)))
		{
			if (i >= size)
			{
				size *= 2;
				a = realloc_aligned(a, size + sizeof(struct alloc));
				s = (char *)(a + 1);
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

	struct alloc *a = malloc_aligned(size + sizeof(struct alloc));
	add_this_alloc(a, STRING_TAG);

	char *s = (char *)(a + 1);

	(void)is->get(is);

	for (int i = 0;; i++)
	{
		if (is->peek(is) != '"')
		{
			if (i >= size)
			{
				size *= 2;
				a = realloc_aligned(a, size + sizeof(struct alloc));
				s = (char *)(a + 1);
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
			s[i] = '\0';
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
	else if (closurep(v))
	{
		struct closure *c = (void *)(v ^ CLOSURE_TAG);
		printf("closure %p taking %d argument(s) and capturing %d value(s)\n",
		       c->function, c->args->num_required, c->num_captured);
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

	if (c == '\'' || c == '`' || c == ',' || c == '#')
	{
		is->get(is);

		if (c == ',' && is->peek(is) == '@')
		{
			// This is actually a splice
			is->get(is);
			c = '@';
		}
		else if (c == '#' && is->peek(is) == '\'')
		{
			is->get(is);
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
		case '#':
			symbol = symval("function");
			break;
		default:
			is->showpos(is, stderr);
			err("Something went wrong parsing a reader macro");
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

void set_cons_info(value_t cons, int line, char *name)
{
	if (!consp(cons))
		return;

	struct cons *ca = (void *)(cons ^ CONS_TAG);

	ca->line = line;
	ca->name = name;
}

value_t readn(struct istream *is)
{
	value_t first = nil;
	value_t *last = &first;

	value_t read_val;

	while (read1(is, &read_val))
	{
		int line;
		char *file;

		is->getpos(is, &line, &file);
		*last = cons(read_val, nil);
		set_cons_info(*last, line, file);

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

static value_t strptrval(char *str, int tag)
{
	value_t v;

	struct alloc *al = malloc_aligned(strlen(str) + 1 + sizeof(struct alloc));
	add_this_alloc(al, SYMBOL_TAG);

	char *a = (char *)(al + 1);

	strcpy(a, str);
	v = (value_t)a;
	v |= tag;

	return v;
}

value_t strval(char *str)
{
	return strptrval(str, STRING_TAG);
}

value_t symval(char *str)
{
	return strptrval(str, SYMBOL_TAG);
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
	return consp(v) || stringp(v) || symbolp(v) || closurep(v);
}

bool closurep(value_t v)
{
	return (v & HEAP_MASK) == CLOSURE_TAG;
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

int cons_line(value_t val)
{
	if (!consp(val))
		return 0;

	struct cons *c = (void *)(val ^ CONS_TAG);

	return c->line;
}

char *cons_file(value_t val)
{
	if (!consp(val))
		return NULL;

	struct cons *c = (void *)(val ^ CONS_TAG);

	return c->name;
}

value_t create_closure(void *code, struct args *args, int ncaptures)
{
	struct closure_alloc *ca = malloc_aligned(sizeof(struct closure_alloc) +
	                                          ncaptures * sizeof(value_t));

	ca->closure.function = code;
	ca->closure.args = args;
	ca->closure.num_captured = ncaptures;

	add_this_alloc(&ca->alloc, CLOSURE_TAG);

	return (value_t)(&ca->closure) | CLOSURE_TAG;
}

void set_closure_capture_variable(int index, value_t value, value_t closure)
{
	if (!closurep(closure))
		return;

	struct closure *c = (void *)(closure ^ CLOSURE_TAG);

	c->data[index] = value;
}

value_t cxdr(value_t v, int index)
{
	if (!listp(v) || index >= length(v))
		return nil;

	for (int i = 0; i < index; i++)
	{
		v = cdr(v);
	}

	return v;
}

value_t *cxdrref(value_t *v, int index)
{
	if (!listp(*v) || index >= length(*v))
		return NULL;

	value_t *p = v;

	for (int i = 0; i < index; i++)
	{
		p = cdrref(*p);
	}

	return p;
}

value_t deep_copy(value_t val)
{
	if (integerp(val) || val == nil || val == t)
	{
		return val;
	}
	else if (symbolp(val))
	{
		return symval((char *)(val ^ SYMBOL_TAG));
	}
	else if (stringp(val))
	{
		return strval((char *)(val ^ STRING_TAG));
	}
	else if (consp(val))
	{
		return cons(deep_copy(car(val)), deep_copy(cdr(val)));
	}
	else if (closurep(val))
	{
		struct closure *c = (void *)(val ^ CLOSURE_TAG);
		value_t new = create_closure(c->function, c->args, c->num_captured);
		struct closure *new_c = (void *)(new ^ CLOSURE_TAG);

		for (int i = 0; i < c->num_captured; i++)
		{
			new_c->data[i] = deep_copy(c->data[i]);
		}

		return new;
	}
	else
	{
		err("Don't know how to deep copy this, sorry... please report this bug :)");
	}
}
