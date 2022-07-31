#pragma once

#include "error.h"
#include "istream.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#define INT_MASK 0b11
#define INT_TAG 0b00

#define CHAR_MASK 0xff
#define CHAR_TAG 0b00001111

#define BOOL_MASK 0b1111111
#define BOOL_TAG 0b0011111

#define HEAP_MASK 0b111

#define CONS_TAG 0b001
#define CLASS_TAG 0b010
#define STRING_TAG 0b011
#define SYMBOL_TAG 0b101
#define CLOSURE_TAG 0b110

struct cons;

/// Represents a Lisp value
typedef unsigned int value_t;

struct cons
{
	value_t car, cdr;
	
	/// Line of the input file from where this was parsed, 0 if it was created
	/// in Lisp.
	int line;

	/// Description of where the cons was parsed from, or NULL if generated in
	/// code.
	char *name;
};

/**
 * Represents how many arguments a function takes.
 */
struct args
{
	/// The minimum valid number of arguments
	int num_required;

	/// The number of optional values
	int num_optional;

	/// Does this function accept variadic arguments? If `true`, any arguments
	/// after the required and optional arguments will be `cons`-ed to a list
	/// and passed as a final argument.
	bool variadic;

	/// The default values for the optional arguments, as expressions. These
	/// should be evaluated at the call site. They are known not to reference
	/// anything that could clash with scope at the call site.
	struct optional_argument
	{
		/// The default value of this argument
		value_t value;

		/// The name of this argument as a symbol
		value_t name;
	} optional_arguments[];
};

struct closure
{
	/// How many arguments does this closure take
	struct args *args;
	/// How many free variables does it capture (i.e. length of `data`)
	int num_captured;
	/// The function pointer itself
	void *function;

	/// This will be passed in edi.
	value_t data[];
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
	unsigned int mark : 24;   // + 3 = 16

	// Whatever else
};

extern struct alloc *first_a, *last_a;

struct cons_alloc
{
	struct alloc alloc;
	struct cons cons;
};

struct closure_alloc
{
	struct alloc alloc;
	struct closure closure;
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

void add_to_pool(value_t form);

void del_alloc(struct alloc *alloc);

/**
 * @returns true if pool is still alive (in scope).
 */
bool pool_alive(unsigned char pool);

bool startswith(struct istream *s, char *pattern);

struct error readsym(struct istream *is, value_t *val) WARN_UNUSED;
struct error readstr(struct istream *is, value_t *val) WARN_UNUSED;
struct error readlist(struct istream *is, value_t *val) WARN_UNUSED;
struct error readint(struct istream *is, value_t *val) WARN_UNUSED;

/**
 * Read a quoted form, including `'` (quote) `\`` (backquote) and `,` (unquote)
 */
struct error readquote(struct istream *is, value_t *val) WARN_UNUSED;

value_t intval(int i);
value_t strval(char *str);
value_t symval(char *str);
value_t cons(value_t car, value_t cdr);
value_t merge2(value_t front, value_t back);
struct error read1(struct istream *is, value_t *val) WARN_UNUSED;
value_t read(struct istream *is);
value_t readn(struct istream *is);

value_t car(value_t v);
value_t cdr(value_t v);
/// Return a pointer to the "nil" tail of the list, or NULL if you do
/// something stupid.
value_t *nilptr(value_t val);
value_t *carref(value_t v);
value_t *cdrref(value_t v);
/// @returns the `index`-th `cdr`
value_t cxdr(value_t v, int index);
/// @returns a reference to the `index`-th `cdr`
value_t *cxdrref(value_t *v, int index);

value_t deep_copy(value_t val);

int cons_line(value_t val);
char *cons_file(value_t val);

bool integerp(value_t v);
bool symbolp(value_t v);
bool stringp(value_t v);
bool consp(value_t v);
bool listp(value_t v);
bool nilp(value_t v);
bool heapp(value_t v);
bool closurep(value_t v);
int length(value_t v);
value_t elt(value_t v, int index);

void printval(value_t v, int depth);

bool symstreq(value_t sym, char *str);

value_t create_closure(void *code, struct args *args, int ncaptures);

/**
 * Set the `index`th capture variable of `closure`. This should really only be
 * called when creating a new closure.
 */
void set_closure_capture_variable(int index, value_t value, value_t closure);

extern value_t nil;
extern value_t t;
