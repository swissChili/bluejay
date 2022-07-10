#include "log.h"
#include "kint.h"
#include "stdarg.h"
#include "vga.h"

void kprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	while (*format)
	{
		if (*format == '%')
		{
			format++;

			switch (*format)
			{
			case 'd': {
				uint x = (uint)va_arg(args, uint);
				vga_putd(x);
				break;
			}

			case 'p':
				vga_put('0');
				vga_put('x');

			case 'x': {
				// consider hex always unsigned
				uint x = (uint)va_arg(args, uint);
				if (x)
					vga_putx(x);
				else
					vga_put('0');
				break;
			}

			case 's': {
				char *s = va_arg(args, char *);
				vga_write(s);
				break;
			}

			case 'c': {
				char s = va_arg(args, int);
				vga_put(s);
				break;
			}
			case 'b': {
				uint v = va_arg(args, uint);
				if (v)
					vga_write("true");
				else
					vga_write("false");
			}
			}
			format++;
		}
		else
		{
			vga_put(*format);
			format++;
		}
	}

	va_end(args);
}

void kassert_int(bool condition, const char *message, const char *file,
				 const int line)
{
	if (!condition)
	{
		vga_set_color(LIGHT_RED, BLACK);
		kprintf(ERROR "ASSERTION FAILED: %s:%d\n%s\n", file, line, message);

		while (1)
		{
		}
	}
}
