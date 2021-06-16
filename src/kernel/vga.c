#include "vga.h"
#include "io.h"
#include "paging.h"
#include "log.h"

static uint cursor_x = 0;
static uint cursor_y = 0;

static uchar color = WHITE;

static ushort *fb = (ushort *)PHYS_TO_VIRT(0xB8000);

#define BUFFER_SIZE 16
static char buffer[BUFFER_SIZE];
static uint buffer_index = 0;
static bool in_escape = false;

static void move_cursor()
{
	ushort pos = cursor_y * 80 + cursor_x;

	outb(0x3d4, 0x0e);
	outb(0x3d5, pos >> 8);
	outb(0x3d4, 0x0f);
	outb(0x3d5, pos & 0xff);
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
	color = (bg << 4) | (fg & 0xf);
}

void got_escape()
{
	if (buffer[0] != '[')
		return;

	int c = parse_int(buffer+1);

	static const char ansi_to_vga_colors[] =
	{
		[30] = BLACK,
		RED,
		GREEN,
		LIGHT_BROWN,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		[90] = LIGHT_GREY,
		LIGHT_RED,
		LIGHT_GREEN,
		LIGHT_BROWN,
		LIGHT_BLUE,
		LIGHT_MAGENTA,
		LIGHT_CYAN,
		WHITE,
	};

	if (c == 0)
	{
		color = WHITE;
	}
	else if ((c >= 30 && c <= 37) || (c >= 90 && c <= 97))
	{
		color &= 0xf0;
		color |= ansi_to_vga_colors[c];
	}
	else if ((c >= 40 && c <= 47) || (c >= 100 && c <= 107))
	{
		color &= 0x0f;
		color |= ansi_to_vga_colors[c - 10] << 4;
	}
}

void vga_put(char c)
{
	if (in_escape)
	{
		if (buffer_index >= BUFFER_SIZE || c == 'm')
		{
			// For now we are only supporting color escape sequences.
			if (c == 'm')
				got_escape();

			// Escape sequence is too long, sorry. Failing silently.
			in_escape = false;
			buffer_index = 0;
			memset(buffer, 0, sizeof(buffer));
		}
		else
		{
			buffer[buffer_index++] = c;
		}

		return;
	}

	switch (c)
	{
	case '\b':
		if (cursor_x > 0)
			cursor_x--;
		fb[cursor_y * 80 + cursor_x] = ' ' | (color << 8);
		break;
	case '\t':
		cursor_x = (cursor_x + 8) & ~7;
		break;
	case '\n':
		cursor_y++;
	case '\r':
		cursor_x = 0;
		break;
	case 033:
		in_escape = true;
		break;
	default:
		fb[cursor_y * 80 + cursor_x] = c | (color << 8);
		cursor_x++;
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
	for (int i = 0; i < 25 * 80; i++)
	{
		fb[i] = ' ' | (WHITE << 8);
	}
	
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

	do
	{
		str[i++] = (d % 10) + '0';
		d /= 10;
	} while (d > 0 && i < 48); // should never be more than 48 digits anyway

	for (uint j = i; j; j--)
	{
		vga_put(str[j - 1]);
	}
}

static bool vga_put_nibble(uchar n, bool first)
{
	//	if (first && n == 0)
	//		return true;

	if (n <= 9)
		vga_put('0' + n);
	else
		vga_put('A' + n - 10);

	return false;
}

void vga_putx(uint x)
{
	bool first = false;

	for (int shift = 24; shift >= 0; shift -= 8)
	{
		uchar byte = x >> shift;

		first = vga_put_nibble((byte & 0xf0) >> 4, first);
		first = vga_put_nibble(byte & 0x0f, first);
	}
}

void init_vga()
{
	// Enable and set max scan line
	outb(0x3D4, 0x09);
	outb(0x3D5, 15);

	// Cursor end line
	outb(0x3D4, 0x0B);
	outb(0x3D5, 15);

	// Cursor start line
	outb(0x3D4, 0x0A);
	outb(0x3D5, 14);
}
