#include "syscall.h"
#include "log.h"
#include "pic.h"

void do_syscall(struct registers *regs)
{
	kprintf(INFO "Syscall executed: %d\n", regs->eax);
}

void init_syscall()
{
	add_interrupt_handler(SYSCALL_INT, do_syscall);
}
