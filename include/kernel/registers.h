#pragma once

#include "kint.h"

/**
 * Represents the full execution state before an interrupt.
 */
struct registers
{
	uint ds;
	uint edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint interrupt_number, error_code;
	uint eip, cs, eflags, useresp, ss;
};
