#pragma once

#include "lisp.h"
#include <dasm_proto.h>
#include <stdbool.h>
#include <stdint.h>

enum namespace
{
	/// A function
	NS_FUNCTION,
	/// A macro
	NS_MACRO,
	/// An anonymous function (a lambda/closure)
	NS_ANONYMOUS,
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

struct loaded_file
{
	char *resolved_path;
	struct loaded_file *previous;
};

struct environment
{
	struct function *first;
	struct loaded_file *first_loaded;
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

	/// Name that the current function should be referred to by, e.g. `recurse`
	/// for a lambda.
	char *current_function_name;

	int num_vars, num_args;
	/// Most recently defined variable
	struct variable *first;
	int npc;
	int nextpc;
	bool *stack_slots;
	/// Number of slots allocated in `stack_slots`
	int num_stack_slots;
	/// Number of entries used in `stack_slots`
	int num_stack_entries;
	/// Number of closure slots total (allocated as V_FREE variables)
	int num_closure_slots;
};

void compile_expression(struct environment *env, struct local *local,
                        value_t val, dasm_State **Dst);

/**
 * Compile a function
 * @param args The function args and body, e.g. `((b c) d)`
 * @param namespace The function namespace.
 * @param env The environment.
 * @param local_out The local environment generated for this function will be
 * returned here. NULL if you do not care about it being returned (you probably
 * should since you need to free the stack slot allocation map).
 * @param local_parent Parent local environment, only needed for closures. NULL
 * if no parent.
 * @param nargs The number of arguments for this function will be returned here.
 * NULL if you don't care about it.
 * @returns The compiled function state. You should probably give this to
 * `add_function` or something similar.
 */
struct dasm_State *compile_function(value_t args, enum namespace namespace,
                                    struct environment *env, struct local *local_out,
                                    struct local *local_parent, int *nargs, char *name);

void compile_variable(struct variable *v, dasm_State *Dst);

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
 * Deletes the memory allocated in `local`. Does not actually call `free()` on
 * `local` itself, but frees the variables and stack slots associated with it.
 */
void del_local(struct local *local);

/**
 * Deletes the memory allocated in `env`. Does not actually call `free()` on
 * `env` itself.
 */
void del_env(struct environment *env);

/**
 * Walk `body` and reserve space in `local` for any variable declarations.
 */
void walk_and_alloc(struct local *local, value_t body);
// Compile top-level declaration
void compile_tl(value_t val, struct environment *env);

/**
 * Compile a file in a new environment.
 * @param filename The path to the file.
 * @param ok Set to `true` if loading succeeds, `false` otherwise. If `ok` is
 * NULL it is ignored.
 * @returns The environment for the compiled file, or an empty environment if
 * `ok` was set to `false` (i.e. the file could not be compiled).
 */
struct environment compile_file(char *filename, bool *ok);

struct function *find_function(struct environment *env, char *name);
struct variable *add_variable(struct local *local, enum var_type type,
                              char *name, int number);

/**
 * Find a variable in `local` with name `name`.
 * @returns The variable, NULL if not found.
 */
struct variable *find_variable(struct local *local, char *name);

void destroy_local(struct local *local);

/**
 * Like `apply` in lisp, calls func with list args and returns the result.
 */
value_t call_list(struct function *func, value_t list);
value_t call_list_closure(struct closure *c, value_t list);

/**
 * Load a lisp file into the current environment.
 * @returns `true` if succesful, `false` otherwise.
 */
bool load(struct environment *env, char *path);

/**
 * Mark a file `path` as loaded in the environment. `path` will be expanded with
 * `readlink`. You can pass a temporary string here, memory will be allocated by
 * this function as needed.
 */
void add_load(struct environment *env, char *path);
