#pragma once

struct initrd_global_header
{
	unsigned int magic;
	unsigned long num_files;
} __attribute__((packed));

struct initrd_file_header
{
	char name[64];
	unsigned long size;
};

/* that's it! */
