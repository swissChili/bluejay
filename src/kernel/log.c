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

			case 'x': {
				// consider hex always unsigned
				uint x = (uint)va_arg(args, uint);
				vga_putx(x);
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
		kprintf("ASSERTION FAILED: %s:%d\n%s\n", file, line, message);

		while (1)
		{
		}
	}
}
