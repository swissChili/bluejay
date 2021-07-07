#pragma once

#include <dasm_proto.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

/* Platform specific definitions */

// Must return an address aligned to 8 bytes
void *malloc_aligned(size_t size);
void *realloc_aligned(void *addr, size_t size);
void free_aligned(void *addr);

void *link_program(dasm_State **Dst);

extern ssize_t readlink(const char *pathname, char *buf, size_t buf_size);
bool file_exists(const char *path);

char *read_input_line(char *prompt);
void add_line_to_history(char *line);

#define THREAD_LOCAL
