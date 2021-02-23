#pragma once

#include "kint.h"
#include "registers.h"

#define VIRT_TO_PHYS(virt) ((uint)(virt) - 0xC0000000)
#define PHYS_TO_VIRT(phys) ((uint)(phys) + 0xC0000000)
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_PAGE_NUMBER (KERNEL_VIRTUAL_BASE >> 22)

/* defined in switch_table.s */
extern uint load_page_directory(uint table_address);
extern void enable_paging();

void *_kmalloc(size_t size, bool align, void **phys);
void *kmalloc(size_t size);
void *kmalloc_a(size_t size);
void *kmalloc_ap(size_t size, void **p);

void init_paging();

void page_fault(struct registers *regs);
