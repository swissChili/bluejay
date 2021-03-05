#pragma once

#include "kint.h"

struct multiboot
{
	uint flags;
	uint mem_lower;
	uint mem_upper;
	uint boot_device;
	char *cmdline;
	uint mods_count;
	uint **mods_addr;
	uint num;
	uint size;
	uint addr;
	uint shndx;
	uint mmap_length;
	uint mmap_addr;
	uint drives_length;
	uint drives_addr;
	uint config_table;
	uint boot_loader_name;
	uint apm_table;
	uint vbe_control_info;
	uint vbe_mode_info;
	uint vbe_mode;
	uint vbe_interface_seg;
	uint vbe_interface_off;
	uint vbe_interface_len;
};

struct multiboot make_multiboot_physical(struct multiboot *old);
