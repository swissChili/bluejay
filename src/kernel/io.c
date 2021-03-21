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
	asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
	return ret;
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
	int i = 0;
	for (; *a; i++)
	{
	}

	return i;
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

void io_wait()
{
	asm volatile("outb %0, $0x80" ::"a"(0));
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
