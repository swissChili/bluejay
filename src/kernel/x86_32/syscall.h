#pragma once

#include "registers.h"

#define SYSCALL_INT 0x80 // copy linux :)

void do_syscall(struct registers *regs);
void init_syscall();
