#include "io.h"
#include "kbd.h"
#include "log.h"
#include "pic.h"

bool pressed_keys[LAST_KBD_KEY];
char special_key_mappings[LAST_KBD_KEY - FIRST_KBD_KEY] = {
	[KBD_ENTER - FIRST_KBD_KEY] = '\n',
	[KBD_SPACEBAR - FIRST_KBD_KEY] = ' ',
	[KBD_TAB - FIRST_KBD_KEY] = '\t',
	[KBD_BACKSPACE - FIRST_KBD_KEY] = '\b',
};

void outb(ushort port, uchar val)
{
	asm volatile("outb %1, %0" : : "dN"(port), "a"(val));
}

uchar inb(ushort port)
{
	uchar ret;
	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

ushort inw(ushort port)
{
	ushort ret;
	asm("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

struct ext2_dir_contains_data
{
	char *name;
	uint len;
	bool found;
};

void outl(ushort port, uint val)
{
	asm("outl %1, %0" : : "dN"(port), "a"(val));
}

uint inl(ushort port)
{
	uint ret;
	asm("inl %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
}

void outw(ushort port, ushort val)
{
	asm("outw %1, %0" :: "dN"(port), "a"(val));
}

void __attribute__((noinline)) nop()
{
	asm("nop");
}

void *memset(void *s, int c, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((uchar *)s)[i] = c;
	}
	return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
	{
		((uchar *)dest)[i] = ((uchar *)src)[i];
	}
	return dest;
}

void strcpy(char *dest, char *src)
{
	memcpy(dest, src, strlen(src) + 1);
}

uint strlen(char *a)
{
	uint i = 0;
	for (; a[i]; i++)
	{
	}

	return i;
}

uint strnlen(char *s, size_t len)
{
	int i;

	for (i = 0; s[i] && i < len; i++)
	{
	}

	return i;
}

int strncmp(char *a, char *b, size_t len)
{
	int al = strnlen(a, len), bl = strnlen(b, len);

	if (al != bl)
		return bl - al;

	for (int i = 0; i < al; i++)
	{
		if (a[i] != b[i])
			return -1;
	}
	return 0;
}

int strcmp(char *a, char *b)
{
	int al = strlen(a), bl = strlen(b);

	if (al != bl)
		return bl - al;

	for (int i = 0; i < al; i++)
	{
		if (a[i] != b[i])
			return -1;
	}
	return 0;
}

uchar kbd_scan_code()
{
	return inb(KBD_DATA_PORT);
}

static bool kbd_shift_pressed()
{
	return pressed_keys[KBD_LEFT_SHIFT] || pressed_keys[KBD_RIGHT_SHIFT] ||
		   pressed_keys[KBD_CAPS_LOCK];
}

void kbd_handle_input(struct registers *registers)
{
	uchar byte = kbd_scan_code();

	struct kbd_scan_codes code = scan_code_set_1[byte];

	if (code.ascii && !code.brk)
	{
		kprintf("%c", kbd_shift_pressed() && code.up_symbol ? code.up_symbol
															: code.symbol);
	}
	else if (!code.brk && special_key_mappings[code.symbol - FIRST_KBD_KEY])
	{
		kprintf("%c", special_key_mappings[code.symbol - FIRST_KBD_KEY]);
	}
	pressed_keys[code.symbol] = !code.brk;
}

void init_kbd()
{
	memset(pressed_keys, 0, LAST_KBD_KEY);
	add_interrupt_handler(33, kbd_handle_input);
}

bool isdigit(char c)
{
	return c >= '0' && c <= '9';
}

uint parse_int(char *string)
{
	uint number = 0;

	for (; isdigit(*string); string++)
	{
		number *= 10;
		number += *string - '0';
	}

	return number;
}
