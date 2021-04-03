#pragma once

#include "kint.h"
#include "registers.h"

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

struct task
{
	int id;
	struct process *proc;
	uint stack_top_p; // stack frame PHYSICAL address
	uint esp, ebp, eip;
};

struct ll_task_i
{
	struct ll_task_i *next;
	struct task task;
};

// extern struct process processes[1024];
// extern struct ll_task_i *first_task, *current_task;

extern void init_tasks();
struct process *get_process(uint pid);

int get_process_id();
int get_task_id();

// For compatibility I guess
#define getpid get_process_id

void spawn_thread(void (*function)());
extern void switch_task();
