#pragma once

#include <stdbool.h>

extern unsigned int cons_magic;

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

struct tag
{
	unsigned int type : 3;
	unsigned int length : 29;
} __attribute__ ((packed));

struct cons;

union value_type {
	int int_val;
	float float_val;
	struct cons *cons_val;
	char *symbol_val; // interned
	char *string_val;
};

struct value
{
	struct tag tag;
	union value_type value;
} __attribute__ ((packed));

struct cons
{
	int magic;
	int marked; // must be reserved
	struct value car, cdr;
};

struct alloc_list
{
	int type;
	union value_type data;
	struct alloc_list *next, *prev;
};

struct istream
{
	void *data;

	// These two return -1 on error
	int (*peek) (struct istream *s);
	int (*get) (struct istream *s);

	int (*read) (struct istream *s, char *buffer, int size);
};

bool startswith (struct istream *s, const char *pattern);

bool readsym (struct istream *is, struct value *val);
bool readstr (struct istream *is, struct value *val);

struct value cons (struct value car, struct value cdr);
bool read1 (struct istream *is, struct value *val);
struct value read (struct istream);
struct value readn (struct istream);

void printval (struct value v, int depth);

struct istream *new_stristream (char *str, int length);
// same as above but null terminated
struct istream *new_stristream_nt (char *str);
void del_stristream(struct istream *stristream);
