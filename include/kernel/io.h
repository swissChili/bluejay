#pragma once

#include "kint.h"
#include "registers.h"

/**
 * IO port for keyboard commands.
 */
#define KBD_CMD_PORT 0x64

/**
 * IO port for keyboard data
 */
#define KBD_DATA_PORT 0x60

/**
 * Write a byte to a port
 */
void outb(ushort port, uchar val);

/**
 * Write a double word to a port.
 */
void outl(ushort port, uint val);

/**
 * Read a byte from a port.
 */
uchar inb(ushort port);

/**
 * Read a word from a port.
 */
ushort inw(ushort port);

/**
 * Write a word to a port.
 */
void outw(ushort port, ushort val);

/**
 * Just waste some time.
 */
void nop();

/**
 * Read a double word from a port
 */
uint inl(ushort port);

/**
 * Set n bytes of s to c.
 */
void *memset(void *s, int c, size_t n);

/**
 * Copy n bytes from src to dest.
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * Copy null terminated string src to dest.
 */
void strcpy(char *dest, char *src);

/**
 * Compare two strings. This might not work like the libc function, so be
 * careful.
 * @returns 0 if equal, non-0 otherwise.
 */
int strcmp(char *a, char *b);
/**
 * @returns the length of null-terminated string a.
 */
uint strlen(char *a);
uint strnlen(char *s, size_t len);
int strncmp(char *a, char *b, size_t len);

bool isdigit(char c);
/**
 * Ignores trailing non-digit characters.
 * @returns 0 if string is not a valid decimal integer
 */
uint parse_int(char *string);

/**
 * Read a scan code from the keyboard
 */
uchar kbd_scan_code();

/**
 * Keyboard IRQ handler
 */
void kbd_handle_input(struct registers *registers);

/**
 * Set up keyboard driver.
 */
void init_kbd();
