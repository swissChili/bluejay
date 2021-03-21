#pragma once

#include "kint.h"
#include "registers.h"

#define VIRT_TO_PHYS(virt) ((uint)(virt) - 0xC0000000)
#define PHYS_TO_VIRT(phys) ((void *)(phys) + 0xC0000000)
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_PAGE_NUMBER (KERNEL_VIRTUAL_BASE >> 22)

/* defined in switch_table.s */
extern uint load_page_directory(uint table_address);
extern void enable_paging();
extern uint kernel_page_directory[1024];

void init_paging();

void map_page_to(uint *dir, void *virt, void *frame_p, bool writable, bool user);
void alloc_frame(uint *page_table_entry, bool user, bool writable);
void alloc_page(uint *dir, uint *page);
void alloc_kernel_page(uint *page);
void page_fault(struct registers *regs);
