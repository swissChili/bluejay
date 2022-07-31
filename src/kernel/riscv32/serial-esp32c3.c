#include "serial.h"
#include "kint.h"

extern int uart_tx_one_char(char byte);

/**
 * @param c Pointer to input
 * @returns OK or not
 */
extern bool uart_rx_one_char(char *c);

/**
 * @param uart_no UART number
 */
extern void uart_tx_flush(uchar uart_no);

void init_serial()
{
}

void serial_put(char byte)
{
	uart_tx_one_char(byte);
}

void serial_write(char *string)
{
	while (*string)
	{
		serial_put(*string++);
	}
}
