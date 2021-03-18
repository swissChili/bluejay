#pragma once

#include "kint.h"

// NOTE: uses kmalloc_a for now, this is bad because we will probably
// run out of memory pretty quick, but malloc() doesn't support alignment
// yet, so here we are.
// dir is the VIRTUAL address, return is also virtual
uint *clone_page_directory(uint *dir);
// here, both return and argument are PHYSICAL
uint clone_page_table(uint table);
