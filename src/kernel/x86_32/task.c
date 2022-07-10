#include "task.h"
#include "alloc.h"
#include "io.h"
#include "log.h"
#include "paging.h"
#include "pic.h"

struct process processes[1024] = {0};
struct ll_task_i *first_task = NULL, *last_task = NULL, *current_task = NULL;
static uint next_task_id = 0;

bool tasks_initialized = false;

void _init_tasks(struct registers *regs);

void init_tasks()
{
	add_interrupt_handler(INIT_TASKS_INTERRUPT, _sys_init_tasks_h);

	asm("int $0x81");
}

void _sys_init_tasks_h(struct registers *regs)
{
	_init_tasks(regs);
}

void _init_tasks(struct registers *regs)
{
	processes[0] = (struct process){
		.exists = true,
		.id = 0,
		.ring = 0,
		.uid = 0,
		.page_directory_p = VIRT_TO_PHYS(kernel_page_directory),
		// Obviously this isn't the actual stack position, but we want
		// it to grow down from 4 gb so we will pretend that the first
		// task has its stack at exactly 4gb and work from
		// there. Because the new stack will be mapped to any random
		// frame, it doesn't actually matter where we put it, we just
		// want somewhere that won't collide with any user space stuff
		// or our heap.
		.last_stack_pos = 0xFFFFF000,
	};
	strcpy(processes[0].name, "kernel");

	first_task = last_task = current_task = malloc(sizeof(struct ll_task_i));

	first_task->next = NULL;
	first_task->prev = NULL;
	memset(&first_task->task, 0, sizeof(struct task));
	first_task->task = (struct task){
		.proc = &processes[0],
		.state = *regs,
		.id = next_task_id++,
		.waiting = false,
	};

	tasks_initialized = true;
}

struct process *get_process(uint pid)
{
	if (pid < 1024)
		return &processes[pid];
	else
		return NULL;
}

int get_task_id()
{
	return current_task->task.id;
}

int get_process_id()
{
	return current_task->task.proc->id;
}

void spawn_thread(void (*function)(void *), void *data)
{
	asm("cli");

	kprintf(DEBUG "Spawning thread %p, data=%p\n", function, data);

	struct process *proc = current_task->task.proc;
	// Virtual address of page directory (in kernel memory)
	uint *dir_v = PHYS_TO_VIRT(proc->page_directory_p);

	// Virtual location of new stack, with space reserved for argument
	// and return address to kill_this_thread().
	uint new_stack_base_v = proc->last_stack_pos;
	proc->last_stack_pos -= 0x1000;

	// Alloc a new page in the current process mapping to the new stack
	alloc_page(dir_v, (void *)proc->last_stack_pos);

	kprintf(INFO "new_stack_base_v = %p\n", new_stack_base_v);
	new_stack_base_v -= sizeof(uint);
	*((uint *)new_stack_base_v) = (size_t)data;
	new_stack_base_v -= sizeof(uint);
	*((uint *)new_stack_base_v) = (size_t)&kill_this_thread;

	kprintf(DEBUG "Set stack\n");

	struct ll_task_i *ll_task = malloc(sizeof(struct ll_task_i));
	memset(ll_task, 0, sizeof(struct ll_task_i));
	struct task *task = &ll_task->task;
	// New task is basically the same as the old one but with just a
	// few changes
	*task = current_task->task;

	// Namely a new TID
	task->id = next_task_id++;
	// And stack, frame, and instruction pointers
	task->state.ebp = task->state.esp = new_stack_base_v;
	task->state.eip = (uint)function;
	task->waiting = false;

	last_task->next = ll_task;
	ll_task->prev = last_task;
	last_task = ll_task;

	asm("sti");
}

void kill_this_thread()
{
	asm("cli");

	if (current_task->prev == NULL && current_task->next == NULL)
	{
		kpanic("You may not kill the last task in the kernel");
	}

	struct ll_task_i *task = first_task,
		*old_task = current_task;

	if (current_task->prev != NULL)
		current_task->prev->next = current_task->next;

	if (current_task->next != NULL)
	{
		// If this is NULL, task will be first_task, which can't be
		// the current task because we know there are more than one
		// task, and this is the last one.
		current_task->next->prev = current_task->prev;
		task = current_task->next;
	}

	if (current_task == first_task)
		first_task = current_task->next;

	if (current_task == last_task)
		last_task = current_task->prev;

	current_task = task;
	free(old_task);

	switch_to_task(&current_task->task);

	asm("sti");
}

extern void _switch_to_task(uint page_directory, struct registers ctx);
#if 0
{
	asm("mov %0, %%ecx" :: "g"(page_directory));
	asm("mov %ecx, %cr3");
	// "ctx" will be at the top of the stack.
	asm("iret");
}
#endif

void switch_to_task(struct task *task)
{
	_switch_to_task(task->proc->page_directory_p, task->state);
	__builtin_unreachable();
}

void _do_switch_task(struct registers regs)
{
	// Resetting eflags in _switch_to_task iret will switch this back
	asm("cli");

	kprintf(DEBUG "switching tasks\n");

	// save context for this task
	current_task->task.state = regs;

	struct ll_task_i *original = current_task;

	do
	{
		if (current_task->next == NULL)
		{
			current_task = first_task;
		}
		else
		{
			// Continue the next task
			current_task = current_task->next;
		}
	}
	while (current_task->task.waiting);

	if (current_task == original && original->task.waiting)
	{
		kpanic("All tasks are waiting for I/O. There must be at least 1 task using CPU at all times.");
	}

	switch_to_task(&current_task->task);
}

void set_waiting(int tid, bool waiting)
{
	asm("cli");

	for (struct ll_task_i *t = first_task; t != NULL; t = t->next)
	{
		if (t->task.id == tid)
		{
			t->task.waiting = waiting;
			break;
		}
	}

	asm("sti");
}

void switch_task(struct registers ctx)
{
	if (tasks_initialized)
		_do_switch_task(ctx);
}