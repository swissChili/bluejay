#include "kint.h"
#include "serial.h"

/* Base address for QEMU */
/* TODO: Abstract for ESP32C3 */
#define UART_BASE (void *)0x10000000
#define UART_LSR_EMPTY_MASK 0x40

/* UART transmitter holding register */
uchar *uart_thr = UART_BASE;

/* UART line status register */
uchar *uart_lsr = UART_BASE + 5;

void init_serial()
{
}

void serial_put(char byte)
{
	while (*uart_lsr ^ UART_LSR_EMPTY_MASK)
	{
		/* block until UART clear */
	}

	*uart_thr = byte;
}

void serial_write(char *string)
{
	while (*string)
	{
		serial_put(*string++);
	}
}
