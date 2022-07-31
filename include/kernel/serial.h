#pragma once

/**
 * Initialize the default serial device.
 */
void init_serial();
void serial_put(char byte);
void serial_write(char *string);
