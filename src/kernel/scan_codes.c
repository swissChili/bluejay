#include "kbd.h"


const struct kbd_scan_codes scan_code_set_1[0xff] =
{
	[41] = { .ascii = true, .symbol = '`',
		.up_symbol = '~', .prefix = 169 },
	[169] = { .ascii = true, .symbol = '`',
		.up_symbol = '~', .prefix = 169, .brk = true },
	[2] = { .ascii = true, .symbol = '1',
		.up_symbol = '!', .prefix = 130 },
	[130] = { .ascii = true, .symbol = '1',
		.up_symbol = '!', .prefix = 130, .brk = true },
	[3] = { .ascii = true, .symbol = '2',
		.up_symbol = '@', .prefix = 131 },
	[131] = { .ascii = true, .symbol = '2',
		.up_symbol = '@', .prefix = 131, .brk = true },
	[4] = { .ascii = true, .symbol = '3',
		.up_symbol = '#', .prefix = 132 },
	[132] = { .ascii = true, .symbol = '3',
		.up_symbol = '#', .prefix = 132, .brk = true },
	[5] = { .ascii = true, .symbol = '4',
		.up_symbol = '$', .prefix = 133 },
	[133] = { .ascii = true, .symbol = '4',
		.up_symbol = '$', .prefix = 133, .brk = true },
	[6] = { .ascii = true, .symbol = '5',
		.up_symbol = '%', .prefix = 134 },
	[134] = { .ascii = true, .symbol = '5',
		.up_symbol = '%', .prefix = 134, .brk = true },
	[7] = { .ascii = true, .symbol = '6',
		.up_symbol = '^', .prefix = 135 },
	[135] = { .ascii = true, .symbol = '6',
		.up_symbol = '^', .prefix = 135, .brk = true },
	[8] = { .ascii = true, .symbol = '7',
		.up_symbol = '&', .prefix = 136 },
	[136] = { .ascii = true, .symbol = '7',
		.up_symbol = '&', .prefix = 136, .brk = true },
	[9] = { .ascii = true, .symbol = '8',
		.up_symbol = '*', .prefix = 137 },
	[137] = { .ascii = true, .symbol = '8',
		.up_symbol = '*', .prefix = 137, .brk = true },
	[10] = { .ascii = true, .symbol = '9',
		.up_symbol = '(', .prefix = 138 },
	[138] = { .ascii = true, .symbol = '9',
		.up_symbol = '(', .prefix = 138, .brk = true },
	[11] = { .ascii = true, .symbol = '0',
		.up_symbol = ')', .prefix = 139 },
	[139] = { .ascii = true, .symbol = '0',
		.up_symbol = ')', .prefix = 139, .brk = true },
	[12] = { .ascii = true, .symbol = '-',
		.up_symbol = '_', .prefix = 140 },
	[140] = { .ascii = true, .symbol = '-',
		.up_symbol = '_', .prefix = 140, .brk = true },
	[13] = { .ascii = true, .symbol = '=',
		.up_symbol = '+', .prefix = 141 },
	[141] = { .ascii = true, .symbol = '=',
		.up_symbol = '+', .prefix = 141, .brk = true },
	[14] = { .ascii = false, .symbol = KBD_BACKSPACE,
		.up_symbol = NULL, .prefix = 142 },
	[142] = { .ascii = false, .symbol = KBD_BACKSPACE,
		.up_symbol = NULL, .prefix = 142, .brk = true },
	[15] = { .ascii = false, .symbol = KBD_TAB,
		.up_symbol = NULL, .prefix = 143 },
	[143] = { .ascii = false, .symbol = KBD_TAB,
		.up_symbol = NULL, .prefix = 143, .brk = true },
	[16] = { .ascii = true, .symbol = 'q',
		.up_symbol = 'Q', .prefix = 144 },
	[144] = { .ascii = true, .symbol = 'q',
		.up_symbol = 'Q', .prefix = 144, .brk = true },
	[17] = { .ascii = true, .symbol = 'w',
		.up_symbol = 'W', .prefix = 145 },
	[145] = { .ascii = true, .symbol = 'w',
		.up_symbol = 'W', .prefix = 145, .brk = true },
	[18] = { .ascii = true, .symbol = 'e',
		.up_symbol = 'E', .prefix = 146 },
	[146] = { .ascii = true, .symbol = 'e',
		.up_symbol = 'E', .prefix = 146, .brk = true },
	[19] = { .ascii = true, .symbol = 'r',
		.up_symbol = 'R', .prefix = 147 },
	[147] = { .ascii = true, .symbol = 'r',
		.up_symbol = 'R', .prefix = 147, .brk = true },
	[20] = { .ascii = true, .symbol = 't',
		.up_symbol = 'T', .prefix = 148 },
	[148] = { .ascii = true, .symbol = 't',
		.up_symbol = 'T', .prefix = 148, .brk = true },
	[21] = { .ascii = true, .symbol = 'y',
		.up_symbol = 'Y', .prefix = 149 },
	[149] = { .ascii = true, .symbol = 'y',
		.up_symbol = 'Y', .prefix = 149, .brk = true },
	[22] = { .ascii = true, .symbol = 'u',
		.up_symbol = 'U', .prefix = 150 },
	[150] = { .ascii = true, .symbol = 'u',
		.up_symbol = 'U', .prefix = 150, .brk = true },
	[23] = { .ascii = true, .symbol = 'i',
		.up_symbol = 'I', .prefix = 151 },
	[151] = { .ascii = true, .symbol = 'i',
		.up_symbol = 'I', .prefix = 151, .brk = true },
	[24] = { .ascii = true, .symbol = 'o',
		.up_symbol = 'O', .prefix = 152 },
	[152] = { .ascii = true, .symbol = 'o',
		.up_symbol = 'O', .prefix = 152, .brk = true },
	[25] = { .ascii = true, .symbol = 'p',
		.up_symbol = 'P', .prefix = 153 },
	[153] = { .ascii = true, .symbol = 'p',
		.up_symbol = 'P', .prefix = 153, .brk = true },
	[26] = { .ascii = true, .symbol = '[',
		.up_symbol = '{', .prefix = 154 },
	[154] = { .ascii = true, .symbol = '[',
		.up_symbol = '{', .prefix = 154, .brk = true },
	[27] = { .ascii = true, .symbol = ']',
		.up_symbol = '}', .prefix = 155 },
	[155] = { .ascii = true, .symbol = ']',
		.up_symbol = '}', .prefix = 155, .brk = true },
	[58] = { .ascii = false, .symbol = KBD_CAPS_LOCK,
		.up_symbol = NULL, .prefix = 186 },
	[186] = { .ascii = false, .symbol = KBD_CAPS_LOCK,
		.up_symbol = NULL, .prefix = 186, .brk = true },
	[30] = { .ascii = true, .symbol = 'a',
		.up_symbol = 'A', .prefix = 158 },
	[158] = { .ascii = true, .symbol = 'a',
		.up_symbol = 'A', .prefix = 158, .brk = true },
	[31] = { .ascii = true, .symbol = 's',
		.up_symbol = 'S', .prefix = 159 },
	[159] = { .ascii = true, .symbol = 's',
		.up_symbol = 'S', .prefix = 159, .brk = true },
	[32] = { .ascii = true, .symbol = 'd',
		.up_symbol = 'D', .prefix = 160 },
	[160] = { .ascii = true, .symbol = 'd',
		.up_symbol = 'D', .prefix = 160, .brk = true },
	[33] = { .ascii = true, .symbol = 'f',
		.up_symbol = 'F', .prefix = 161 },
	[161] = { .ascii = true, .symbol = 'f',
		.up_symbol = 'F', .prefix = 161, .brk = true },
	[34] = { .ascii = true, .symbol = 'g',
		.up_symbol = 'G', .prefix = 162 },
	[162] = { .ascii = true, .symbol = 'g',
		.up_symbol = 'G', .prefix = 162, .brk = true },
	[35] = { .ascii = true, .symbol = 'h',
		.up_symbol = 'H', .prefix = 163 },
	[163] = { .ascii = true, .symbol = 'h',
		.up_symbol = 'H', .prefix = 163, .brk = true },
	[36] = { .ascii = true, .symbol = 'j',
		.up_symbol = 'J', .prefix = 164 },
	[164] = { .ascii = true, .symbol = 'j',
		.up_symbol = 'J', .prefix = 164, .brk = true },
	[37] = { .ascii = true, .symbol = 'k',
		.up_symbol = 'K', .prefix = 165 },
	[165] = { .ascii = true, .symbol = 'k',
		.up_symbol = 'K', .prefix = 165, .brk = true },
	[38] = { .ascii = true, .symbol = 'l',
		.up_symbol = 'L', .prefix = 166 },
	[166] = { .ascii = true, .symbol = 'l',
		.up_symbol = 'L', .prefix = 166, .brk = true },
	[39] = { .ascii = true, .symbol = ';',
		.up_symbol = ':', .prefix = 167 },
	[167] = { .ascii = true, .symbol = ';',
		.up_symbol = ':', .prefix = 167, .brk = true },
	[40] = { .ascii = true, .symbol = '\'',
		.up_symbol = '"', .prefix = 168 },
	[168] = { .ascii = true, .symbol = '\'',
		.up_symbol = '"', .prefix = 168, .brk = true },
	[28] = { .ascii = false, .symbol = KBD_ENTER,
		.up_symbol = NULL, .prefix = 156 },
	[156] = { .ascii = false, .symbol = KBD_ENTER,
		.up_symbol = NULL, .prefix = 156, .brk = true },
	[42] = { .ascii = false, .symbol = KBD_LEFT_SHIFT,
		.up_symbol = NULL, .prefix = 170 },
	[170] = { .ascii = false, .symbol = KBD_LEFT_SHIFT,
		.up_symbol = NULL, .prefix = 170, .brk = true },
	[44] = { .ascii = true, .symbol = 'z',
		.up_symbol = 'Z', .prefix = 172 },
	[172] = { .ascii = true, .symbol = 'z',
		.up_symbol = 'Z', .prefix = 172, .brk = true },
	[45] = { .ascii = true, .symbol = 'x',
		.up_symbol = 'X', .prefix = 173 },
	[173] = { .ascii = true, .symbol = 'x',
		.up_symbol = 'X', .prefix = 173, .brk = true },
	[46] = { .ascii = true, .symbol = 'c',
		.up_symbol = 'C', .prefix = 174 },
	[174] = { .ascii = true, .symbol = 'c',
		.up_symbol = 'C', .prefix = 174, .brk = true },
	[47] = { .ascii = true, .symbol = 'v',
		.up_symbol = 'V', .prefix = 175 },
	[175] = { .ascii = true, .symbol = 'v',
		.up_symbol = 'V', .prefix = 175, .brk = true },
	[48] = { .ascii = true, .symbol = 'b',
		.up_symbol = 'B', .prefix = 176 },
	[176] = { .ascii = true, .symbol = 'b',
		.up_symbol = 'B', .prefix = 176, .brk = true },
	[49] = { .ascii = true, .symbol = 'n',
		.up_symbol = 'N', .prefix = 177 },
	[177] = { .ascii = true, .symbol = 'n',
		.up_symbol = 'N', .prefix = 177, .brk = true },
	[50] = { .ascii = true, .symbol = 'm',
		.up_symbol = 'M', .prefix = 178 },
	[178] = { .ascii = true, .symbol = 'm',
		.up_symbol = 'M', .prefix = 178, .brk = true },
	[51] = { .ascii = true, .symbol = ',',
		.up_symbol = '<', .prefix = 179 },
	[179] = { .ascii = true, .symbol = ',',
		.up_symbol = '<', .prefix = 179, .brk = true },
	[52] = { .ascii = true, .symbol = '.',
		.up_symbol = '>', .prefix = 180 },
	[180] = { .ascii = true, .symbol = '.',
		.up_symbol = '>', .prefix = 180, .brk = true },
	[53] = { .ascii = true, .symbol = '/',
		.up_symbol = '?', .prefix = 181 },
	[181] = { .ascii = true, .symbol = '/',
		.up_symbol = '?', .prefix = 181, .brk = true },
	[54] = { .ascii = false, .symbol = KBD_RIGHT_SHIFT,
		.up_symbol = NULL, .prefix = 182 },
	[182] = { .ascii = false, .symbol = KBD_RIGHT_SHIFT,
		.up_symbol = NULL, .prefix = 182, .brk = true },
	[29] = { .ascii = false, .symbol = KBD_LEFT_CTRL,
		.up_symbol = NULL, .prefix = 157 },
	[157] = { .ascii = false, .symbol = KBD_LEFT_CTRL,
		.up_symbol = NULL, .prefix = 157, .brk = true },
	[56] = { .ascii = false, .symbol = KBD_LEFT_ALT,
		.up_symbol = NULL, .prefix = 184 },
	[184] = { .ascii = false, .symbol = KBD_LEFT_ALT,
		.up_symbol = NULL, .prefix = 184, .brk = true },
	[57] = { .ascii = false, .symbol = KBD_SPACEBAR,
		.up_symbol = NULL, .prefix = 185 },
	[185] = { .ascii = false, .symbol = KBD_SPACEBAR,
		.up_symbol = NULL, .prefix = 185, .brk = true },
	[69] = { .ascii = false, .symbol = KBD_NUM_LOCK,
		.up_symbol = NULL, .prefix = 197 },
	[197] = { .ascii = false, .symbol = KBD_NUM_LOCK,
		.up_symbol = NULL, .prefix = 197, .brk = true },
	[71] = { .ascii = false, .symbol = KBD_KEYPAD_7,
		.up_symbol = NULL, .prefix = 199 },
	[199] = { .ascii = false, .symbol = KBD_KEYPAD_7,
		.up_symbol = NULL, .prefix = 199, .brk = true },
	[75] = { .ascii = false, .symbol = KBD_KEYPAD_4,
		.up_symbol = NULL, .prefix = 203 },
	[203] = { .ascii = false, .symbol = KBD_KEYPAD_4,
		.up_symbol = NULL, .prefix = 203, .brk = true },
	[79] = { .ascii = false, .symbol = KBD_KEYPAD_1,
		.up_symbol = NULL, .prefix = 207 },
	[207] = { .ascii = false, .symbol = KBD_KEYPAD_1,
		.up_symbol = NULL, .prefix = 207, .brk = true },
	[72] = { .ascii = false, .symbol = KBD_KEYPAD_8,
		.up_symbol = NULL, .prefix = 200 },
	[200] = { .ascii = false, .symbol = KBD_KEYPAD_8,
		.up_symbol = NULL, .prefix = 200, .brk = true },
	[76] = { .ascii = false, .symbol = KBD_KEYPAD_5,
		.up_symbol = NULL, .prefix = 204 },
	[204] = { .ascii = false, .symbol = KBD_KEYPAD_5,
		.up_symbol = NULL, .prefix = 204, .brk = true },
	[80] = { .ascii = false, .symbol = KBD_KEYPAD_2,
		.up_symbol = NULL, .prefix = 208 },
	[208] = { .ascii = false, .symbol = KBD_KEYPAD_2,
		.up_symbol = NULL, .prefix = 208, .brk = true },
	[82] = { .ascii = false, .symbol = KBD_KEYPAD_0,
		.up_symbol = NULL, .prefix = 210 },
	[210] = { .ascii = false, .symbol = KBD_KEYPAD_0,
		.up_symbol = NULL, .prefix = 210, .brk = true },
	[55] = { .ascii = false, .symbol = KBD_KEYPAD_TIMES,
		.up_symbol = NULL, .prefix = 183 },
	[183] = { .ascii = false, .symbol = KBD_KEYPAD_TIMES,
		.up_symbol = NULL, .prefix = 183, .brk = true },
	[73] = { .ascii = false, .symbol = KBD_KEYPAD_9,
		.up_symbol = NULL, .prefix = 201 },
	[201] = { .ascii = false, .symbol = KBD_KEYPAD_9,
		.up_symbol = NULL, .prefix = 201, .brk = true },
	[77] = { .ascii = false, .symbol = KBD_KEYPAD_6,
		.up_symbol = NULL, .prefix = 205 },
	[205] = { .ascii = false, .symbol = KBD_KEYPAD_6,
		.up_symbol = NULL, .prefix = 205, .brk = true },
	[81] = { .ascii = false, .symbol = KBD_KEYPAD_3,
		.up_symbol = NULL, .prefix = 209 },
	[209] = { .ascii = false, .symbol = KBD_KEYPAD_3,
		.up_symbol = NULL, .prefix = 209, .brk = true },
	[83] = { .ascii = false, .symbol = KBD_KEYPAD_DOT,
		.up_symbol = NULL, .prefix = 211 },
	[211] = { .ascii = false, .symbol = KBD_KEYPAD_DOT,
		.up_symbol = NULL, .prefix = 211, .brk = true },
	[74] = { .ascii = false, .symbol = KBD_KEYPAD_MINUS,
		.up_symbol = NULL, .prefix = 202 },
	[202] = { .ascii = false, .symbol = KBD_KEYPAD_MINUS,
		.up_symbol = NULL, .prefix = 202, .brk = true },
	[78] = { .ascii = false, .symbol = KBD_KEYPAD_PLUS,
		.up_symbol = NULL, .prefix = 206 },
	[206] = { .ascii = false, .symbol = KBD_KEYPAD_PLUS,
		.up_symbol = NULL, .prefix = 206, .brk = true },
	[1] = { .ascii = false, .symbol = KBD_ESC,
		.up_symbol = NULL, .prefix = 129 },
	[129] = { .ascii = false, .symbol = KBD_ESC,
		.up_symbol = NULL, .prefix = 129, .brk = true },
	[59] = { .ascii = false, .symbol = KBD_F1,
		.up_symbol = NULL, .prefix = 187 },
	[187] = { .ascii = false, .symbol = KBD_F1,
		.up_symbol = NULL, .prefix = 187, .brk = true },
	[60] = { .ascii = false, .symbol = KBD_F2,
		.up_symbol = NULL, .prefix = 188 },
	[188] = { .ascii = false, .symbol = KBD_F2,
		.up_symbol = NULL, .prefix = 188, .brk = true },
	[61] = { .ascii = false, .symbol = KBD_F3,
		.up_symbol = NULL, .prefix = 189 },
	[189] = { .ascii = false, .symbol = KBD_F3,
		.up_symbol = NULL, .prefix = 189, .brk = true },
	[62] = { .ascii = false, .symbol = KBD_F4,
		.up_symbol = NULL, .prefix = 190 },
	[190] = { .ascii = false, .symbol = KBD_F4,
		.up_symbol = NULL, .prefix = 190, .brk = true },
	[63] = { .ascii = false, .symbol = KBD_F5,
		.up_symbol = NULL, .prefix = 191 },
	[191] = { .ascii = false, .symbol = KBD_F5,
		.up_symbol = NULL, .prefix = 191, .brk = true },
	[64] = { .ascii = false, .symbol = KBD_F6,
		.up_symbol = NULL, .prefix = 192 },
	[192] = { .ascii = false, .symbol = KBD_F6,
		.up_symbol = NULL, .prefix = 192, .brk = true },
	[65] = { .ascii = false, .symbol = KBD_F7,
		.up_symbol = NULL, .prefix = 193 },
	[193] = { .ascii = false, .symbol = KBD_F7,
		.up_symbol = NULL, .prefix = 193, .brk = true },
	[66] = { .ascii = false, .symbol = KBD_F8,
		.up_symbol = NULL, .prefix = 194 },
	[194] = { .ascii = false, .symbol = KBD_F8,
		.up_symbol = NULL, .prefix = 194, .brk = true },
	[67] = { .ascii = false, .symbol = KBD_F9,
		.up_symbol = NULL, .prefix = 195 },
	[195] = { .ascii = false, .symbol = KBD_F9,
		.up_symbol = NULL, .prefix = 195, .brk = true },
	[68] = { .ascii = false, .symbol = KBD_F10,
		.up_symbol = NULL, .prefix = 196 },
	[196] = { .ascii = false, .symbol = KBD_F10,
		.up_symbol = NULL, .prefix = 196, .brk = true },
	[87] = { .ascii = false, .symbol = KBD_F11,
		.up_symbol = NULL, .prefix = 215 },
	[215] = { .ascii = false, .symbol = KBD_F11,
		.up_symbol = NULL, .prefix = 215, .brk = true },
	[88] = { .ascii = false, .symbol = KBD_F12,
		.up_symbol = NULL, .prefix = 216 },
	[216] = { .ascii = false, .symbol = KBD_F12,
		.up_symbol = NULL, .prefix = 216, .brk = true },
	[70] = { .ascii = false, .symbol = KBD_SCROLL_LOCK,
		.up_symbol = NULL, .prefix = 198 },
	[198] = { .ascii = false, .symbol = KBD_SCROLL_LOCK,
		.up_symbol = NULL, .prefix = 198, .brk = true },
	[43] = { .ascii = true, .symbol = '\\',
		.up_symbol = '|', .prefix = 171 },
	[171] = { .ascii = true, .symbol = '\\',
		.up_symbol = '|', .prefix = 171, .brk = true },
};


const struct kbd_scan_codes scan_code_set_3[0xff] =
{
	[14] = { .ascii = true, .symbol = '`',
		.up_symbol = '~', .prefix = 240 },
	[22] = { .ascii = true, .symbol = '1',
		.up_symbol = '!', .prefix = 240 },
	[30] = { .ascii = true, .symbol = '2',
		.up_symbol = '@', .prefix = 240 },
	[38] = { .ascii = true, .symbol = '3',
		.up_symbol = '#', .prefix = 240 },
	[37] = { .ascii = true, .symbol = '4',
		.up_symbol = '$', .prefix = 240 },
	[46] = { .ascii = true, .symbol = '5',
		.up_symbol = '%', .prefix = 240 },
	[54] = { .ascii = true, .symbol = '6',
		.up_symbol = '^', .prefix = 240 },
	[61] = { .ascii = true, .symbol = '7',
		.up_symbol = '&', .prefix = 240 },
	[62] = { .ascii = true, .symbol = '8',
		.up_symbol = '*', .prefix = 240 },
	[70] = { .ascii = true, .symbol = '9',
		.up_symbol = '(', .prefix = 240 },
	[69] = { .ascii = true, .symbol = '0',
		.up_symbol = ')', .prefix = 240 },
	[78] = { .ascii = true, .symbol = '-',
		.up_symbol = '_', .prefix = 240 },
	[85] = { .ascii = true, .symbol = '=',
		.up_symbol = '+', .prefix = 240 },
	[102] = { .ascii = false, .symbol = KBD_BACKSPACE,
		.up_symbol = NULL, .prefix = 240 },
	[13] = { .ascii = false, .symbol = KBD_TAB,
		.up_symbol = NULL, .prefix = 240 },
	[21] = { .ascii = true, .symbol = 'q',
		.up_symbol = 'Q', .prefix = 240 },
	[29] = { .ascii = true, .symbol = 'w',
		.up_symbol = 'W', .prefix = 240 },
	[36] = { .ascii = true, .symbol = 'e',
		.up_symbol = 'E', .prefix = 240 },
	[45] = { .ascii = true, .symbol = 'r',
		.up_symbol = 'R', .prefix = 240 },
	[44] = { .ascii = true, .symbol = 't',
		.up_symbol = 'T', .prefix = 240 },
	[53] = { .ascii = true, .symbol = 'y',
		.up_symbol = 'Y', .prefix = 240 },
	[60] = { .ascii = true, .symbol = 'u',
		.up_symbol = 'U', .prefix = 240 },
	[67] = { .ascii = true, .symbol = 'i',
		.up_symbol = 'I', .prefix = 240 },
	[68] = { .ascii = true, .symbol = 'o',
		.up_symbol = 'O', .prefix = 240 },
	[77] = { .ascii = true, .symbol = 'p',
		.up_symbol = 'P', .prefix = 240 },
	[84] = { .ascii = true, .symbol = '[',
		.up_symbol = '{', .prefix = 240 },
	[91] = { .ascii = true, .symbol = ']',
		.up_symbol = '}', .prefix = 240 },
	[88] = { .ascii = false, .symbol = KBD_CAPS_LOCK,
		.up_symbol = NULL, .prefix = 240 },
	[28] = { .ascii = true, .symbol = 'a',
		.up_symbol = 'A', .prefix = 240 },
	[27] = { .ascii = true, .symbol = 's',
		.up_symbol = 'S', .prefix = 240 },
	[35] = { .ascii = true, .symbol = 'd',
		.up_symbol = 'D', .prefix = 240 },
	[43] = { .ascii = true, .symbol = 'f',
		.up_symbol = 'F', .prefix = 240 },
	[52] = { .ascii = true, .symbol = 'g',
		.up_symbol = 'G', .prefix = 240 },
	[51] = { .ascii = true, .symbol = 'h',
		.up_symbol = 'H', .prefix = 240 },
	[59] = { .ascii = true, .symbol = 'j',
		.up_symbol = 'J', .prefix = 240 },
	[66] = { .ascii = true, .symbol = 'k',
		.up_symbol = 'K', .prefix = 240 },
	[75] = { .ascii = true, .symbol = 'l',
		.up_symbol = 'L', .prefix = 240 },
	[76] = { .ascii = true, .symbol = ';',
		.up_symbol = ':', .prefix = 240 },
	[82] = { .ascii = true, .symbol = '\'',
		.up_symbol = '"', .prefix = 240 },
	[90] = { .ascii = false, .symbol = KBD_ENTER,
		.up_symbol = NULL, .prefix = 240 },
	[18] = { .ascii = false, .symbol = KBD_LEFT_SHIFT,
		.up_symbol = NULL, .prefix = 240 },
	[26] = { .ascii = true, .symbol = 'z',
		.up_symbol = 'Z', .prefix = 240 },
	[34] = { .ascii = true, .symbol = 'x',
		.up_symbol = 'X', .prefix = 240 },
	[33] = { .ascii = true, .symbol = 'c',
		.up_symbol = 'C', .prefix = 240 },
	[42] = { .ascii = true, .symbol = 'v',
		.up_symbol = 'V', .prefix = 240 },
	[50] = { .ascii = true, .symbol = 'b',
		.up_symbol = 'B', .prefix = 240 },
	[49] = { .ascii = true, .symbol = 'n',
		.up_symbol = 'N', .prefix = 240 },
	[58] = { .ascii = true, .symbol = 'm',
		.up_symbol = 'M', .prefix = 240 },
	[65] = { .ascii = true, .symbol = ',',
		.up_symbol = '<', .prefix = 240 },
	[73] = { .ascii = true, .symbol = '.',
		.up_symbol = '>', .prefix = 240 },
	[74] = { .ascii = true, .symbol = '/',
		.up_symbol = '?', .prefix = 240 },
	[89] = { .ascii = false, .symbol = KBD_RIGHT_SHIFT,
		.up_symbol = NULL, .prefix = 240 },
	[17] = { .ascii = false, .symbol = KBD_LEFT_CTRL,
		.up_symbol = NULL, .prefix = 240 },
	[25] = { .ascii = false, .symbol = KBD_LEFT_ALT,
		.up_symbol = NULL, .prefix = 240 },
	[41] = { .ascii = false, .symbol = KBD_SPACEBAR,
		.up_symbol = NULL, .prefix = 240 },
	[57] = { .ascii = false, .symbol = KBD_RIGHT_ALT,
		.up_symbol = NULL, .prefix = 240 },
	[88] = { .ascii = false, .symbol = KBD_RIGHT_CTRL,
		.up_symbol = NULL, .prefix = 240 },
	[103] = { .ascii = false, .symbol = KBD_INSERT,
		.up_symbol = NULL, .prefix = 240 },
	[100] = { .ascii = false, .symbol = KBD_DELETE,
		.up_symbol = NULL, .prefix = 240 },
	[97] = { .ascii = false, .symbol = KBD_LEFT_ARROW,
		.up_symbol = NULL, .prefix = 240 },
	[110] = { .ascii = false, .symbol = KBD_HOME,
		.up_symbol = NULL, .prefix = 240 },
	[101] = { .ascii = false, .symbol = KBD_END,
		.up_symbol = NULL, .prefix = 240 },
	[99] = { .ascii = false, .symbol = KBD_UP_ARROW,
		.up_symbol = NULL, .prefix = 240 },
	[96] = { .ascii = false, .symbol = KBD_DOWN_ARROW,
		.up_symbol = NULL, .prefix = 240 },
	[111] = { .ascii = false, .symbol = KBD_PAGE_UP,
		.up_symbol = NULL, .prefix = 240 },
	[109] = { .ascii = false, .symbol = KBD_PAGE_DOWN,
		.up_symbol = NULL, .prefix = 240 },
	[106] = { .ascii = false, .symbol = KBD_RIGHT_ARROW,
		.up_symbol = NULL, .prefix = 240 },
	[118] = { .ascii = false, .symbol = KBD_NUM_LOCK,
		.up_symbol = NULL, .prefix = 240 },
	[108] = { .ascii = false, .symbol = KBD_KEYPAD_7,
		.up_symbol = NULL, .prefix = 240 },
	[107] = { .ascii = false, .symbol = KBD_KEYPAD_4,
		.up_symbol = NULL, .prefix = 240 },
	[105] = { .ascii = false, .symbol = KBD_KEYPAD_1,
		.up_symbol = NULL, .prefix = 240 },
	[119] = { .ascii = false, .symbol = KBD_KEYPAD_DIV,
		.up_symbol = NULL, .prefix = 240 },
	[117] = { .ascii = false, .symbol = KBD_KEYPAD_8,
		.up_symbol = NULL, .prefix = 240 },
	[115] = { .ascii = false, .symbol = KBD_KEYPAD_5,
		.up_symbol = NULL, .prefix = 240 },
	[114] = { .ascii = false, .symbol = KBD_KEYPAD_2,
		.up_symbol = NULL, .prefix = 240 },
	[112] = { .ascii = false, .symbol = KBD_KEYPAD_0,
		.up_symbol = NULL, .prefix = 240 },
	[126] = { .ascii = false, .symbol = KBD_KEYPAD_TIMES,
		.up_symbol = NULL, .prefix = 240 },
	[125] = { .ascii = false, .symbol = KBD_KEYPAD_9,
		.up_symbol = NULL, .prefix = 240 },
	[116] = { .ascii = false, .symbol = KBD_KEYPAD_6,
		.up_symbol = NULL, .prefix = 240 },
	[122] = { .ascii = false, .symbol = KBD_KEYPAD_3,
		.up_symbol = NULL, .prefix = 240 },
	[113] = { .ascii = false, .symbol = KBD_KEYPAD_DOT,
		.up_symbol = NULL, .prefix = 240 },
	[132] = { .ascii = false, .symbol = KBD_KEYPAD_MINUS,
		.up_symbol = NULL, .prefix = 240 },
	[124] = { .ascii = false, .symbol = KBD_KEYPAD_PLUS,
		.up_symbol = NULL, .prefix = 240 },
	[121] = { .ascii = false, .symbol = KBD_KEYPAD_ENTER,
		.up_symbol = NULL, .prefix = 240 },
	[8] = { .ascii = false, .symbol = KBD_ESC,
		.up_symbol = NULL, .prefix = 240 },
	[7] = { .ascii = false, .symbol = KBD_F1,
		.up_symbol = NULL, .prefix = 240 },
	[15] = { .ascii = false, .symbol = KBD_F2,
		.up_symbol = NULL, .prefix = 240 },
	[23] = { .ascii = false, .symbol = KBD_F3,
		.up_symbol = NULL, .prefix = 240 },
	[31] = { .ascii = false, .symbol = KBD_F4,
		.up_symbol = NULL, .prefix = 240 },
	[39] = { .ascii = false, .symbol = KBD_F5,
		.up_symbol = NULL, .prefix = 240 },
	[47] = { .ascii = false, .symbol = KBD_F6,
		.up_symbol = NULL, .prefix = 240 },
	[55] = { .ascii = false, .symbol = KBD_F7,
		.up_symbol = NULL, .prefix = 240 },
	[63] = { .ascii = false, .symbol = KBD_F8,
		.up_symbol = NULL, .prefix = 240 },
	[71] = { .ascii = false, .symbol = KBD_F9,
		.up_symbol = NULL, .prefix = 240 },
	[79] = { .ascii = false, .symbol = KBD_F10,
		.up_symbol = NULL, .prefix = 240 },
	[86] = { .ascii = false, .symbol = KBD_F11,
		.up_symbol = NULL, .prefix = 240 },
	[94] = { .ascii = false, .symbol = KBD_F12,
		.up_symbol = NULL, .prefix = 240 },
	[87] = { .ascii = false, .symbol = KBD_PRINT_SCREEN,
		.up_symbol = NULL, .prefix = 240 },
	[95] = { .ascii = false, .symbol = KBD_SCROLL_LOCK,
		.up_symbol = NULL, .prefix = 240 },
	[98] = { .ascii = false, .symbol = KBD_PAUSE_BREAK,
		.up_symbol = NULL, .prefix = 240 },
	[92] = { .ascii = true, .symbol = '\\',
		.up_symbol = '|', .prefix = 240 },
};
