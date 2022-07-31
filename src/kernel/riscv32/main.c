#include "kint.h"
#include "serial.h"

void kmain(uint mhartid)
{
	init_serial();

	serial_write("Hello from Bluejay!\r\n");
	
	while (true)
	{
	}
}
