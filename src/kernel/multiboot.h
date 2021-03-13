#pragma once

/* multiboot.h - Multiboot header file. */
/* Copyright (C) 1999,2003,2007,2008,2009,2010  Free Software Foundation, Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL ANY
 *  DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "kint.h"

/* How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH 8192
#define MULTIBOOT_HEADER_ALIGN 4

/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

/* This should be in %eax. */
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

/* Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN 0x00001000

/* Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN 0x00000004

/* Flags set in the ’flags’ member of the multiboot header. */

/* Align all boot modules on i386 page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN 0x00000001

/* Must pass memory information to OS. */
#define MULTIBOOT_MEMORY_INFO 0x00000002

/* Must pass video information to OS. */
#define MULTIBOOT_VIDEO_MODE 0x00000004

/* This flag indicates the use of the address fields in the header. */
#define MULTIBOOT_AOUT_KLUDGE 0x00010000

/* Flags to be set in the ’flags’ member of the multiboot info structure. */

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY 0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE 0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS 0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP 0x00000040

/* Is there drive info? */
#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080

/* Is there a config table? */
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100

/* Is there a boot loader name? */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200

/* Is there a APM table? */
#define MULTIBOOT_INFO_APM_TABLE 0x00000400

/* Is there video information? */
#define MULTIBOOT_INFO_VBE_INFO 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

#ifndef ASM_FILE

struct multiboot_header
{
	/* Must be MULTIBOOT_MAGIC - see above. */
	uint magic;

	/* Feature flags. */
	uint flags;

	/* The above fields plus this one must equal 0 mod 2^32. */
	uint checksum;

	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	uint header_addr;
	uint load_addr;
	uint load_end_addr;
	uint bss_end_addr;
	uint entry_addr;

	/* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	uint mode_type;
	uint width;
	uint height;
	uint depth;
};

/* The symbol table for a.out. */
struct multiboot_aout_symbol_table
{
	uint tabsize;
	uint strsize;
	uint addr;
	uint reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;

/* The section header table for ELF. */
struct multiboot_elf_section_header_table
{
	uint num;
	uint size;
	uint addr;
	uint shndx;
};
typedef struct multiboot_elf_section_header_table
	multiboot_elf_section_header_table_t;

struct multiboot_info
{
	/* Multiboot info version number */
	uint flags;

	/* Available memory from BIOS */
	uint mem_lower;
	uint mem_upper;

	/* "root" partition */
	uint boot_device;

	/* Kernel command line */
	uint cmdline;

	/* Boot-Module list */
	uint mods_count;
	uint mods_addr;

	union {
		multiboot_aout_symbol_table_t aout_sym;
		multiboot_elf_section_header_table_t elf_sec;
	} u;

	/* Memory Mapping buffer */
	uint mmap_length;
	uint mmap_addr;

	/* Drive Info buffer */
	uint drives_length;
	uint drives_addr;

	/* ROM configuration table */
	uint config_table;

	/* Boot Loader Name */
	uint boot_loader_name;

	/* APM table */
	uint apm_table;

	/* Video */
	uint vbe_control_info;
	uint vbe_mode_info;
	ushort vbe_mode;
	ushort vbe_interface_seg;
	ushort vbe_interface_off;
	ushort vbe_interface_len;

	uint framebuffer_addr_low;
	uint framebuffer_addr_high;
	uint framebuffer_pitch;
	uint framebuffer_width;
	uint framebuffer_height;
	uchar framebuffer_bpp;
#define MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED 0
#define MULTIBOOT_FRAMEBUFFER_TYPE_RGB 1
#define MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT 2
	uchar framebuffer_type;
	union {
		struct
		{
			uint framebuffer_palette_addr;
			ushort framebuffer_palette_num_colors;
		};
		struct
		{
			uchar framebuffer_red_field_position;
			uchar framebuffer_red_mask_size;
			uchar framebuffer_green_field_position;
			uchar framebuffer_green_mask_size;
			uchar framebuffer_blue_field_position;
			uchar framebuffer_blue_mask_size;
		};
	};
};
typedef struct multiboot_info multiboot_info_t;

struct multiboot_color
{
	uchar red;
	uchar green;
	uchar blue;
};

/*struct multiboot_mmap_entry
{
	uint size;
	multiboot_uint64_t addr;
	multiboot_uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5
	uint type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t; */

struct multiboot_mod_list
{
	/* the memory used goes from bytes ’mod_start’ to ’mod_end-1’ inclusive */
	uint mod_start;
	uint mod_end;

	/* Module command line */
	uint cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	uint pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

/* APM BIOS info. */
struct multiboot_apm_info
{
	ushort version;
	ushort cseg;
	uint offset;
	ushort cseg_16;
	ushort dseg;
	ushort flags;
	ushort cseg_len;
	ushort cseg_16_len;
	ushort dseg_len;
};

struct multiboot_info make_multiboot_physical(struct multiboot_info *old);

#endif // asm
