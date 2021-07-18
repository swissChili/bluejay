#include "syscall.h"
#include "log.h"
#include "pic.h"
#include "task.h"

#include <sys.h>

void do_syscall(struct registers *regs)
{
	switch (regs->eax)
	{
	case SYS_GIVEUP:
		// easy, just switch tasks
		switch_task();
		break;

	default:
		kprintf(INFO "Syscall executed: %d\n", regs->eax);
	}
}

void init_syscall()
{
	add_interrupt_handler(SYSCALL_INT, do_syscall);
}
