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

struct args *new_args();
struct args *add_optional_arg(struct args *args, value_t name,
                              value_t expression);

/**
 * @returns if `number` is an acceptable number of arguments for `args`.
 */
bool are_args_acceptable(struct args *args, int number);

struct function
{
	char *name;
	struct args *args;
	enum namespace namespace;

	union {
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

	/// Path to the current file
	char *current_file_path;

	int num_vars;
	struct args *args;
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

/**
 * Parse a list of arguments to an args object and add them as V_ARGUMENT
 * variables to `local`. The list should be in the same format as is accepted by
 * `defun`, `defmacro`, `lambda`, etc.
 * @returns NULL if the list is malformed.
 */
struct error list_to_args(struct environment *env, value_t list,
                          struct local *local, struct args **args);

/**
 * Print out `args` to stdout. Useful for debugging.
 */
void display_args(struct args *args);

struct error compile_expression(struct environment *env, struct local *local,
								value_t val, bool tail, dasm_State **Dst) WARN_UNUSED;

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
 * @param args An object representing the arguments this function accepts will
 * be returned here. Set this to NULL if you don't care.
 * @returns The compiled function state. You should probably give this to
 * `add_function` or something similar.
 */
struct error compile_function(value_t args, enum namespace namespace,
							  struct environment *env,
							  struct local *local_out,
							  struct local *local_parent,
							  struct args **ar, char *name, char *path,
							  dasm_State **s) WARN_UNUSED;

struct error compile_variable(struct variable *v, dasm_State *Dst) WARN_UNUSED;

/**
 * Compile a backquoted expression
 */
struct error compile_backquote(struct environment *env, struct local *local,
							   value_t val, dasm_State **Dst) WARN_UNUSED;

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

/**
 * Compile a top level definition
 * @param fname The path to the current file.
 * @param val The expression to compile.
 */
struct error compile_tl(value_t val, struct environment *env, char *fname) WARN_UNUSED;

/**
 * Compile a file in a new environment.
 * @param filename The path to the file.
 * @param ok Set to `true` if loading succeeds, `false` otherwise. If `ok` is
 * NULL it is ignored.
 * @returns The environment for the compiled file, or an empty environment if
 * `ok` was set to `false` (i.e. the file could not be compiled).
 */
struct error compile_file(char *filename, struct environment **env) WARN_UNUSED;

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
 * Load a file relative to another file.
 * @param to The file to load relative to.
 * @param name The name or relative path of the file to load.
 * @param env The environment to load in.
 */
value_t load_relative(struct environment *env, char *to, value_t name);

/**
 * Mark a file `path` as loaded in the environment. `path` will be expanded with
 * `readlink`. You can pass a temporary string here, memory will be allocated by
 * this function as needed.
 */
void add_load(struct environment *env, char *path);
