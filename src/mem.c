#include "mem.h"


void outb(ushort port, uchar val)
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (val));
}

uchar inb(ushort port)
{
	uchar ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

ushort inw(ushort port)
{
	ushort ret;
	asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void *memset(void *s, int c, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((uchar *)s)[i] = c;
	}
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((uchar *)dest)[i] = ((uchar *)src)[i];
	}
}
