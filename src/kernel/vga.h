#pragma once

#include "kint.h"

enum vga_colors
{
	BLACK = 0,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GREY,
	DARK_GREY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	LIGHT_BROWN,
	WHITE,
};

void vga_set_color(enum vga_colors fg, enum vga_colors bg);
void vga_put(char c);
void vga_putd(uint d);
void vga_putx(uint x);
void vga_clear();
void vga_write(char *c);

void init_vga();
