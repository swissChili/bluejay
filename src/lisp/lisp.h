#pragma once

#include "istream.h"
#include <stdbool.h>
#include <stdio.h>

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
	value_t car, cdr;
};

/// Default pool (no pool)
#define NO_POOL 0

/**
 * The max used pool number, don't touch this.
 */
extern unsigned char max_pool;

/**
 * Current allocation pool, default 0 (no pool)
 */
extern unsigned char current_pool;

// It is integral that this be 16 bytes long so that whatever follows it is
// still aligned to 4 bits.
struct alloc
{
	/**
	 * One of the type tags, eg CONS_TAG, etc
	 */
	unsigned int type_tag;     //   4
	struct alloc *prev, *next; // + 8
	/**
	 * Zero if this is not part of a release pool, pool number otherwise.
	 */
	unsigned char pool;       // + 1
	/**
	 * Reserved for the GC.
	 */
	unsigned int mark : 24;   // + 2 = 16

	// Whatever else
};

extern struct alloc *first_a, *last_a;

struct cons_alloc
{
	struct alloc alloc;
	struct cons cons;
};

/**
 * Create a new allocation pool.
 */
unsigned char make_pool();

/**
 * Set the allocation pull
 * @returns the old pool, you should reset this later with pop_pool.
 */
unsigned char push_pool(unsigned char pool);

/**
 * Set the allocation pool and throw away the old value.
 */
void pop_pool(unsigned char pool);

/**
 * @returns true if pool is still alive (in scope).
 */
bool pool_alive(unsigned char pool);

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
bool heapp(value_t v);
int length(value_t v);
value_t elt(value_t v, int index);

void printval(value_t v, int depth);

void err(const char *msg);

bool symstreq(value_t sym, char *str);

extern value_t nil;
extern value_t t;
