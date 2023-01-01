#pragma once

#include "kint.h"
#include "registers.h"

extern bool tasks_initialized;

/**
 * A process. For now there is only one, the kernel.
 */
struct process
{
	bool exists;
	int id; // kernel uses pid 0, which cannot exit
	int ring;
	int uid;
	char name[32];
	uint page_directory_p;
	// most recent (bottom) stack used by a task, next
	// stack should be under this one.
	// NOTE: must be PAGE ALIGNED
	uint last_stack_pos;
};

/**
 * The smallest schedulable unit, a thread of a process.
 */
struct task
{
	int id;
	/// Is this task waiting for I/O?
	bool waiting;
	struct process *proc;
	/// Physical address of the top of the stack.
	uint stack_top_p;
	struct registers state;
};

struct ll_task_i
{
	struct ll_task_i *next, *prev;
	struct task task;
};

// extern struct process processes[1024];
// extern struct ll_task_i *first_task, *current_task;

// Note: interrupts must be enabled BEFORE this for it to work.
void init_tasks();
struct process *get_process(uint pid);

int get_process_id();
int get_task_id();

// For compatibility I guess
#define getpid get_process_id

typedef void (*task_function_t)(void *data);

#define TASK_FUNCTION(f) ((task_function_t)(f))

void spawn_thread(task_function_t function, void *data);

void set_waiting(int tid, bool waiting);

/**
 * Halt the current thread.
 */
void kill_this_thread();

/**
 * Force a task switch
 * @param ctx The context to switch out of
 */
void switch_task(struct registers ctx);

/**
 * Switch to a specific task. Only call in a safe environment (ISR).
 */
void switch_to_task(struct task *task);

/**
 * Internal. Do not call.
 */
void _sys_init_tasks_h(struct registers *regs);
