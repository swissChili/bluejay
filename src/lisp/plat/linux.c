#include "plat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#ifndef NO_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

void *malloc_aligned(size_t size)
{
	// https://www.gnu.org/software/libc/manual/html_node/Aligned-Memory-Blocks.html
	// On glibc malloc() and realloc() return 8-byte aligned addresses.
	return malloc(size);
}

void *realloc_aligned(void *addr, size_t size)
{
	return realloc(addr, size);
}

void free_aligned(void *addr)
{
	free(addr);
}

void *link_program(dasm_State **Dst)
{
	size_t size;
	void *buf;

	dasm_link(Dst, &size);
	buf = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1,
	           0);
	dasm_encode(Dst, buf);

	mprotect(buf, size, PROT_READ | PROT_EXEC);
	return buf;
}

bool file_exists(const char *path)
{
	return access(path, F_OK) == 0;
}

char *read_input_line(char *prompt)
{
#ifndef NO_READLINE
	return readline(prompt);
#else
	return "";
#endif
}

void add_line_to_history(char *line)
{
#ifndef NO_READLINE
	add_history(line);
#endif
}
