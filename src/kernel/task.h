#pragma once

#include "kint.h"
#include "registers.h"

struct process
{
	bool exists;
	int id;
	int ring;
	int uid;
	char name[32];
};

struct task
{
	int id;
	struct process *proc;
	uint stack_top_p; // stack frame PHYSICAL address
	uint esp, ebp, eip;
	uint *page_directory;
	bool kernel;
};

struct ll_task_i
{
	struct ll_task_i *next;
	struct task task;
};

// extern struct process processes[1024];
// extern struct ll_task_i *first_task, *current_task;

void init_tasks(uint kernel_esp, uint kernel_ebp, uint kernel_eip);
struct process *get_process(uint pid);

int get_process_id();
int get_task_id();

// For compatibility I guess
#define getpid get_process_id

void spawn_thread(void (* function)(void *data), void *data);
