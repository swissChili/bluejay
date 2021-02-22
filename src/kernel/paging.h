#pragma once

#include "kint.h"
#include "registers.h"

/* defined in switch_table.s */
extern uint load_page_directory(uint table_address);
extern void enable_paging();

void *_kmalloc(size_t size, bool align, void **phys);
void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_ap(size_t size, void **p);

void initialize_paging();

void page_fault(struct registers *regs);
