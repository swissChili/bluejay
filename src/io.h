#pragma once

#include "kint.h"

void outb(ushort port, uchar val);
uchar inb(ushort port);
ushort inw(ushort port);

void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

void io_wait();
