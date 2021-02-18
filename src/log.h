#pragma once

#include "kint.h"

void kprintf(const char *format, ...);
void kassert_int(bool condition, const char *message, const char *file, const int line);

#define kassert(cond, msg) kassert_int((cond), (msg), __FILE__, __LINE__)
