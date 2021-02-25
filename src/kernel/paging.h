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

void init_paging();

void alloc_frame(uint *page_table_entry, bool user, bool writable);
void alloc_kernel_page(uint *page);
void page_fault(struct registers *regs);
