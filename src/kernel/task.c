#include "task.h"
#include "alloc.h"
#include "io.h"
#include "paging.h"

struct process processes[1024] = {0};
struct ll_task_i *first_task = NULL, *last_task = NULL, *current_task = NULL;

void init_tasks(uint kernel_esp, uint kernel_ebp, uint kernel_eip)
{
	asm volatile("cli");

	processes[0] = (struct process){
		.exists = true,
		.id = 0,
		.ring = 0,
		.uid = 0,
	};
	strcpy(processes[0].name, "kernel");

	first_task = last_task = current_task = malloc(sizeof(struct ll_task_i));

	first_task->next = NULL;
	first_task->task = (struct task){
		.proc = &processes[0],
		.esp = kernel_esp,
		.ebp = kernel_ebp,
		.eip = kernel_eip,
		.kernel = true,
		.page_directory = kernel_page_directory,
	};

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

void spawn_thread(void (* function)(void *data), void *data)
{

}
