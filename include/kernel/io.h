#pragma once

#include "kint.h"
#include "registers.h"

#define KBD_CMD_PORT 0x64
#define KBD_DATA_PORT 0x60

struct kbd_scan_code_info
{
	bool pressed;
	bool escape;
	char key;
};

extern struct kbd_scan_code_info scan_code_table[0xff];

void outb(ushort port, uchar val);
void outl(ushort port, uint val);
uchar inb(ushort port);
ushort inw(ushort port);
uint inl(ushort port);

/* Random string.h stuff, TODO: move to own header */
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void strcpy(char *dest, char *src);
int strcmp(char *a, char *b);
uint strlen(char *a);

bool isdigit(char c);
uint parse_int(char *string);

uchar kbd_scan_code();
void kbd_handle_input(struct registers *registers);
void init_kbd();
