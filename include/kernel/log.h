#pragma once

#include "kint.h"

#define RESET "\033[0m"
#define INFO "[\033[96m INFO  " RESET "] "
#define OKAY "[\033[92m  OK   " RESET "] "
#define ERROR "[\033[91m ERROR " RESET "] "
#define DEBUG "[\033[93m DEBUG " RESET "] "

void kprintf(const char *format, ...);
void kassert_int(bool condition, const char *message, const char *file,
				 const int line);

#define kassert(cond, msg) kassert_int((cond), (msg), __FILE__, __LINE__)
#define kpanic(msg)      \
	kassert(false, msg); \
	__builtin_unreachable()
