#pragma once

#include "kint.h"

void *_kmalloc(size_t size, bool align, void **phys);
void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_ap(size_t size, void **p);

void *malloc(size_t size);
void free(void *mem);
void *realloc(void *mem, size_t size);

void init_allocator();

void test_allocator();
