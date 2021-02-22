#pragma once

#include "kint.h"

struct kbd_scan_codes
{
	bool ascii;
	int symbol;
	int up_symbol;
	uchar prefix;
	bool brk;
};

enum kbd_keys
{
	FIRST_KBD_KEY = 129,
	KBD_BACKSPACE = 129,
	KBD_TAB,
	KBD_CAPS_LOCK,
	KBD_ENTER,
	KBD_LEFT_SHIFT,
	KBD_RIGHT_SHIFT,
	KBD_LEFT_CTRL,
	KBD_LEFT_ALT,
	KBD_SPACEBAR,
	KBD_RIGHT_ALT,
	KBD_RIGHT_CTRL,
	KBD_INSERT,
	KBD_DELETE,
	KBD_LEFT_ARROW,
	KBD_HOME,
	KBD_END,
	KBD_UP_ARROW,
	KBD_DOWN_ARROW,
	KBD_RIGHT_ARROW,
	KBD_NUM_LOCK,
	KBD_KEYPAD_0,
	KBD_KEYPAD_1,
	KBD_KEYPAD_2,
	KBD_KEYPAD_3,
	KBD_KEYPAD_4,
	KBD_KEYPAD_5,
	KBD_KEYPAD_6,
	KBD_KEYPAD_7,
	KBD_KEYPAD_8,
	KBD_KEYPAD_9,
	KBD_KEYPAD_DOT,
	KBD_KEYPAD_PLUS,
	KBD_KEYPAD_MINUS,
	KBD_KEYPAD_TIMES,
	KBD_KEYPAD_DIV,
	KBD_KEYPAD_ENTER,
	KBD_ESC,
	KBD_F1,
	KBD_F2,
	KBD_F3,
	KBD_F4,
	KBD_F5,
	KBD_F6,
	KBD_F7,
	KBD_F8,
	KBD_F9,
	KBD_F10,
	KBD_F11,
	KBD_F12,
	KBD_PRINT_SCREEN,
	KBD_SCROLL_LOCK,
	KBD_PAUSE_BREAK,
	KBD_PAGE_UP,
	KBD_PAGE_DOWN,
	LAST_KBD_KEY,
};

extern const struct kbd_scan_codes scan_code_set_3[0xff];
extern const struct kbd_scan_codes scan_code_set_1[0xff];
extern bool pressed_keys[LAST_KBD_KEY];
extern char special_key_mappings[LAST_KBD_KEY - FIRST_KBD_KEY];
