#include "vga.h"
#include "io.h"
#include "log.h"

static uint cursor_x = 0;
static uint cursor_y = 0;

static ushort color = WHITE;

static ushort *fb = (ushort *)0xB8000;

static void move_cursor()
{
	ushort loc = cursor_y * 80 + cursor_x;
	outb(0x3d4, 14); // Setting high cursor byte
	outb(0x3d4, loc >> 8);

	outb(0x3d4, 15); // low byte
	outb(0x3d4, loc & 0xff);
}

static void scroll()
{
	ushort blank = ' ' | color << 8;

	while (cursor_y >= 25) // end of line
	{
		// scroll everything up
		memcpy(fb, &fb[80], 24 * 80 * 2);

		for (int i = 24 * 80; i < 25 * 80; i++)
		{
			fb[i] = blank;
		}

		cursor_y--;
	}
}

void vga_set_color(enum vga_colors fg, enum vga_colors bg)
{
	color = (bg << 4) | fg & 0xf;
}

void vga_put(char c)
{
	switch (c)
	{
	case '\b':
		if (cursor_x > 0)
			cursor_x--;
		break;
	case '\t':
		cursor_x = (cursor_x + 8) & ~ 7;
		break;
	case '\n':
		cursor_y++;
	case '\r':
		cursor_x = 0;
		break;
	default:
		cursor_x++;
		fb[cursor_y * 80 + cursor_x] = c | (color << 8);
	}

	if (cursor_x >= 80) // off screen
	{
		cursor_x = 0;
		cursor_y++;
	}

	scroll();
	move_cursor();
}

void vga_clear()
{
	memset(fb, 0, 80 * 25 * 2);
	cursor_x = 0;
	cursor_y = 0;
	move_cursor();
}

void vga_write(char *c)
{
	for (int i = 0; c[i]; i++)
		vga_put(c[i]);
}

void vga_putd(uint d)
{
	char str[48];
	memset(str, 0, 48);
	uint i = 0;

	while (d > 0 && i < 48) // should never be more than 48 digits anyway
	{
		str[i++] = (d % 10) + '0';
		d /= 10;
	}

	for (uint j = i; j; j--)
	{
		vga_put(str[j - 1]);
	}
}

static bool vga_put_nibble(uchar n, bool first)
{
	if (first && n == 0)
		return true;
	
	if (n <= 9)
		vga_put('0' + n);
	else
		vga_put('A' + n - 10);

	return false;
}

void vga_putx(uint x)
{
	bool first = true;

	for (uint mask = 0xFF000000, shift = 24; mask; mask >>= 8, shift -= 8)
	{
		uchar byte = (x & mask) >> shift;

		first = vga_put_nibble((byte & 0xf0) >> 8, first);
		first = vga_put_nibble(byte & 0x0f, first);
	}
}
