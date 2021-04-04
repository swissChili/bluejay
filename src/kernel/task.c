#include "task.h"
#include "alloc.h"
#include "io.h"
#include "log.h"
#include "paging.h"

struct process processes[1024] = {0};
struct ll_task_i *first_task = NULL, *last_task = NULL, *current_task = NULL;
static uint next_task_id = 0;

void _init_tasks(uint kernel_esp, uint kernel_ebp, uint kernel_eip)
{
	asm volatile("cli");

	kprintf("_init_tasks\n");

	processes[0] = (struct process){
		.exists = true,
		.id = 0,
		.ring = 0,
		.uid = 0,
		.page_directory_p = VIRT_TO_PHYS(kernel_page_directory),
		// Obviously this isn't the actual stack position, but we want it to
		// grow down from 4 gb so we will pretend that the first task has its
		// stack at exactly 4gb and work from there. Because the new stack will
		// be mapped to any random frame, it doesn't actually matter where we
		// put it, we just want somewhere that won't collide with any user space
		// stuff or our heap.
		.last_stack_pos = 0xFFFFF000,
	};
	strcpy(processes[0].name, "kernel");
	kprintf("in _init_tasks, strlen of 'kernel' is %d\n", strlen("kernel"));

	first_task = last_task = current_task = malloc(sizeof(struct ll_task_i));

	first_task->next = NULL;
	first_task->task = (struct task){
		.proc = &processes[0],
		.esp = kernel_esp,
		.ebp = kernel_ebp,
		.eip = kernel_eip,
		.id = next_task_id++,
	};

	kprintf("Returning from _init_tasks\n");

	asm volatile("sti");
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

void spawn_thread(void (*function)())
{
	asm volatile("cli");

	struct process *proc = current_task->task.proc;
	// Virtual address of page directory (in kernel memory)
	uint *dir_v = PHYS_TO_VIRT(proc->page_directory_p);
	// Virtual location of new stack
	uint new_stack_base_v = proc->last_stack_pos - sizeof(uint);
	proc->last_stack_pos -= 0x1000;

	// Alloc a new page in the current process mapping to the new stack
	alloc_page(dir_v, (void *)proc->last_stack_pos);

	// <TEST>: see if we can assign to the new stack memory (ie: did mapping succeed)
	uint *base = (uint *)new_stack_base_v;

	kprintf("base = 0x%x\n", base);

	*base = 0;
	// </TEST>

	struct ll_task_i *ll_task = malloc(sizeof(struct ll_task_i));
	memset(ll_task, 0, sizeof(struct ll_task_i));
	struct task *task = &ll_task->task;

	task->proc = proc;
	task->id = next_task_id++;
	task->ebp = task->esp = new_stack_base_v;
	task->eip = (uint)function;

	last_task->next = ll_task;
	last_task = ll_task;

	asm volatile("sti");
}

extern void _switch_to_task(uint page_directory, uint eip, uint ebp, uint esp);

void switch_to_task(struct task *task)
{
	_switch_to_task(task->proc->page_directory_p, task->eip, task->ebp,
					task->esp);
	__builtin_unreachable();
}

// WARNING: do not call this manually, it will clobber everything
// except esp, ebp, and eip (obviously). Use switch_task in task_api.s
// instead.
void _do_switch_task(uint eip, uint ebp, uint esp)
{
	// sti is called in switch_to_task
	asm volatile("cli");

	kprintf("\nin _do_switch_task(%d, %d, %d)\n", eip, ebp, esp);

	// save context for this task
	current_task->task.ebp = ebp;
	current_task->task.esp = esp;
	current_task->task.eip = eip;

	if (current_task->next == NULL)
	{
		// Start from the first task if there are more tasks, or just return
		if (current_task == first_task)
			return; // No context switch necessary

		current_task = first_task;
	}
	else
	{
		// Continue the next task
		current_task = current_task->next;
	}

	kprintf("Will switch to task id %d\n", current_task->task.id);

	switch_to_task(&current_task->task);
}
