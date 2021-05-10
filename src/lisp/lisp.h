#pragma once

#include "istream.h"
#include <stdbool.h>
#include <stdio.h>

enum type
{
	T_INT = 0,
	T_FLOAT,
	T_NIL,
	T_SYMBOL,
	T_STRING,
	T_VECTOR,
	T_CLASS,
	T_CONS,
};

#define INT_MASK 0b11
#define INT_TAG 0b00

#define CHAR_MASK 0xff
#define CHAR_TAG 0b00001111

#define BOOL_MASK 0b1111111
#define BOOL_TAG 0b0011111

#define HEAP_MASK 0b111

#define CONS_TAG 0b001
#define VECTOR_TAG 0b010
#define STRING_TAG 0b011
#define SYMBOL_TAG 0b101
#define CLOSURE_TAG 0b110

struct cons;

typedef unsigned int value_t;

struct cons
{
	int magic;
	int marked; // must be reserved
	value_t car, cdr;
};

struct alloc_list
{
	int type;
	union {
		struct cons *cons_val;
	};
	struct alloc_list *next, *prev;
};

bool startswith(struct istream *s, char *pattern);

bool readsym(struct istream *is, value_t *val);
bool readstr(struct istream *is, value_t *val);
bool readlist(struct istream *is, value_t *val);
bool readint(struct istream *is, value_t *val);

value_t intval(int i);
value_t strval(char *str);
value_t cons(value_t car, value_t cdr);
bool read1(struct istream *is, value_t *val);
value_t read(struct istream *is);
value_t readn(struct istream *is);

value_t car(value_t v);
value_t cdr(value_t v);
value_t *carref(value_t v);
value_t *cdrref(value_t v);

bool integerp(value_t v);
bool symbolp(value_t v);
bool stringp(value_t v);
bool consp(value_t v);
bool listp(value_t v);
bool nilp(value_t v);
int length(value_t v);
value_t elt(value_t v, int index);

void printval(value_t v, int depth);

void err(const char *msg);

bool symstreq(value_t sym, char *str);

extern value_t nil;
extern value_t t;
