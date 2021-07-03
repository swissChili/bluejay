#pragma once

#include "lisp.h"
#include <dasm_proto.h>
#include <stdbool.h>
#include <stdint.h>

enum namespace
{
	NS_FUNCTION,
	NS_MACRO,
};

struct function
{
	char *name;
	int nargs; // number of arguments
	enum namespace namespace;

	union
	{
		value_t (*def0)();
		value_t (*def1)(value_t);
		value_t (*def2)(value_t, value_t);
		value_t (*def3)(value_t, value_t, value_t);
		void *code_ptr;
		uintptr_t code_addr;
	};

	struct function *prev;
};

struct environment
{
	struct function *first;
};

enum var_type
{
	V_BOUND,    // Bound local variable
	V_ARGUMENT, // Bound function argument
	V_GLOBAL,   // Global variable
	V_FREE      // Free (lexical) variable
};

struct variable
{
	char *name;
	uintptr_t number;
	enum var_type type;
	struct variable *prev;
};

/// Local environment
struct local
{
	/// Parent environment, NULL if none (root).
	struct local *parent;

	int num_vars;
	/// Most recently defined variable
	struct variable *first;
	int npc;
	int nextpc;
	bool *stack_slots;
	int num_stack_slots, num_stack_entries;
};

void compile_expression(struct environment *env, struct local *local,
                        value_t val, dasm_State **Dst);

/**
 * Compile a backquoted expression
 */
void compile_backquote(struct environment *env, struct local *local,
                       value_t val, dasm_State **Dst);

void compile_expr_to_func(struct environment *env, char *name, value_t val,
                          dasm_State **Dst);

int nextpc(struct local *local, dasm_State **Dst);

// Local utilities
unsigned int local_alloc(struct local *local);
void local_free(struct local *local, unsigned int slot);

/**
 * Walk `body` and reserve space in `local` for any variable declarations.
 */
void walk_and_alloc(struct local *local, value_t body);
// Compile top-level declaration
void compile_tl(value_t val, struct environment *env);
struct environment compile_all(struct istream *is);
struct function *find_function(struct environment *env, char *name);
struct variable *add_variable(struct local *local, enum var_type type,
                              char *name, int number);
// Might return null
struct variable *find_variable(struct local *local, char *name);
void destroy_local(struct local *local);

/**
 * Like `apply` in lisp, calls func with list args and returns the result.
 */
value_t call_list(struct function *func, value_t list);
