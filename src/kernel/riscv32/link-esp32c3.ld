MEMORY
{
	irom (x): org = 0x42000000, len = 0x400000
	drom (r): org = 0x3C000000, len = 0x400000
	ram (rw): org = 0x3FC80000, len = 0x50000
	rtc_ram (rx): org = 0x50000000, len = 0x2000
}

ENTRY(_start)

SECTIONS
{
	.header : AT(0)
	{
		_irom_start = .;
		LONG(0xaedb041d)
		LONG(0xaedb041d)
	} > irom

	.text.entry ORIGIN(irom) + 8 :
	{
		KEEP(*(.text.entry))
	} > irom

	.text :
	{
		*(.text .stub .text.* .gnu.linkonce.t.*)
		*(.gnu.warning)
	}
	. = ALIGN(4);
	PROVIDE (__etext = .);
	PROVIDE (_etext = .);
	PROVIDE (etext = .);
	_irom_size = . - _irom_start;
	
	_drom_start = ORIGIN(drom) + _irom_size;
	.rodata  _drom_start : AT(_irom_size)
	{
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	} > drom

	.rodata1 :
	{
		*(.rodata1)
	}

	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
	} > drom

	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		PROVIDE_HIDDEN (__fini_array_end = .);
	} > drom

	.ctors :
	{
		KEEP (*crtbegin.o(.ctors))
		KEEP (*crtbegin?.o(.ctors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}

	.dtors :
	{
		KEEP (*crtbegin.o(.dtors))
		KEEP (*crtbegin?.o(.dtors))
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
	}

	_drom_size = . - _drom_start;

	.data ORIGIN(ram) : AT(_irom_size + _drom_size) 
	{
		_data_start = .;
		__DATA_BEGIN__ = .;
		*(.data .data.* .gnu.linkonce.d.*)
		*(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*) *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*)
		SORT(CONSTRUCTORS)
	} > ram
	.data1 :
	{
		*(.data1)
	}
	.sdata :
	{
		__SDATA_BEGIN__ = .;
		*(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
		*(.sdata .sdata.* .gnu.linkonce.s.*)
	}
	. = ALIGN(4);
	_edata = .; PROVIDE (edata = .);
	_data_lma = ORIGIN(drom) + LOADADDR(.data);
	_data_size = _edata - _data_start;

	__bss_start = .;
	.sbss           :
	{
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
	}
	.bss            :
	{
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
	}
	. = ALIGN(4);
	__BSS_END__ = .;
	__global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800,
							MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));
	_end = .; PROVIDE (end = .);

	/* Stack */
	.stack :
	{
		__stack_bottom = .;
		__stack_top = ORIGIN(ram) + LENGTH(ram);
		__stack_size_min = 0x4000;
		ASSERT(__stack_bottom + __stack_size_min < __stack_top, "Error: no space for stack");
	}

	/* Stabs debugging sections.  */
	.stab          0 : { *(.stab) }
	.stabstr       0 : { *(.stabstr) }
	.stab.excl     0 : { *(.stab.excl) }
	.stab.exclstr  0 : { *(.stab.exclstr) }
	.stab.index    0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment       0 : { *(.comment) }
	.gnu.build.attributes : { *(.gnu.build.attributes .gnu.build.attributes.*) }
	/* DWARF debug sections.
	Symbols in the DWARF debugging sections are relative to the beginning
	of the section so we begin them at 0.  */
	/* DWARF 1 */
	.debug          0 : { *(.debug) }
	.line           0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo  0 : { *(.debug_srcinfo) }
	.debug_sfnames  0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges  0 : { *(.debug_aranges) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev   0 : { *(.debug_abbrev) }
	.debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end) }
	.debug_frame    0 : { *(.debug_frame) }
	.debug_str      0 : { *(.debug_str) }
	.debug_loc      0 : { *(.debug_loc) }
	.debug_macinfo  0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	/* DWARF 3 */
	.debug_pubtypes 0 : { *(.debug_pubtypes) }
	.debug_ranges   0 : { *(.debug_ranges) }
	/* DWARF Extension.  */
	.debug_macro    0 : { *(.debug_macro) }
	.debug_addr     0 : { *(.debug_addr) }
	.gnu.attributes 0 : { KEEP (*(.gnu.attributes)) }
	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}

/* ESP32C3 ROM functions from ./interface-esp32c3.yml */

/* Functions */
__absvdi2 = 0x40000764;
__absvsi2 = 0x40000768;
__adddf3 = 0x4000076c;
__addsf3 = 0x40000770;
__addvdi3 = 0x40000774;
__addvsi3 = 0x40000778;
__ashldi3 = 0x4000077c;
__ashrdi3 = 0x40000780;
__bswapdi2 = 0x40000784;
__bswapsi2 = 0x40000788;
__clear_cache = 0x4000078c;
__clrsbdi2 = 0x40000790;
__clrsbsi2 = 0x40000794;
__clzdi2 = 0x40000798;
__clzsi2 = 0x4000079c;
__cmpdi2 = 0x400007a0;
__ctzdi2 = 0x400007a4;
__ctzsi2 = 0x400007a8;
__divdc3 = 0x400007ac;
__divdf3 = 0x400007b0;
__divdi3 = 0x400007b4;
__divsc3 = 0x400007b8;
__divsf3 = 0x400007bc;
__divsi3 = 0x400007c0;
__eqdf2 = 0x400007c4;
__eqsf2 = 0x400007c8;
__extendsfdf2 = 0x400007cc;
__ffsdi2 = 0x400007d0;
__ffssi2 = 0x400007d4;
__fixdfdi = 0x400007d8;
__fixdfsi = 0x400007dc;
__fixsfdi = 0x400007e0;
__fixsfsi = 0x400007e4;
__fixunsdfsi = 0x400007e8;
__fixunssfdi = 0x400007ec;
__fixunssfsi = 0x400007f0;
__floatdidf = 0x400007f4;
__floatdisf = 0x400007f8;
__floatsidf = 0x400007fc;
__floatsisf = 0x40000800;
__floatundidf = 0x40000804;
__floatundisf = 0x40000808;
__floatunsidf = 0x4000080c;
__floatunsisf = 0x40000810;
__gcc_bcmp = 0x40000814;
__gedf2 = 0x40000818;
__gesf2 = 0x4000081c;
__gtdf2 = 0x40000820;
__gtsf2 = 0x40000824;
__ledf2 = 0x40000828;
__lesf2 = 0x4000082c;
__lshrdi3 = 0x40000830;
__ltdf2 = 0x40000834;
__ltsf2 = 0x40000838;
__moddi3 = 0x4000083c;
__modsi3 = 0x40000840;
__muldc3 = 0x40000844;
__muldf3 = 0x40000848;
__muldi3 = 0x4000084c;
__mulsc3 = 0x40000850;
__mulsf3 = 0x40000854;
__mulsi3 = 0x40000858;
__mulvdi3 = 0x4000085c;
__mulvsi3 = 0x40000860;
__nedf2 = 0x40000864;
__negdf2 = 0x40000868;
__negdi2 = 0x4000086c;
__negsf2 = 0x40000870;
__negvdi2 = 0x40000874;
__negvsi2 = 0x40000878;
__nesf2 = 0x4000087c;
__paritysi2 = 0x40000880;
__popcountdi2 = 0x40000884;
__popcountsi2 = 0x40000888;
__powidf2 = 0x4000088c;
__powisf2 = 0x40000890;
__subdf3 = 0x40000894;
__subsf3 = 0x40000898;
__subvdi3 = 0x4000089c;
__subvsi3 = 0x400008a0;
__truncdfsf2 = 0x400008a4;
__ucmpdi2 = 0x400008a8;
__udivdi3 = 0x400008ac;
__udivmoddi4 = 0x400008b0;
__udivsi3 = 0x400008b4;
__udiv_w_sdiv = 0x400008b8;
__umoddi3 = 0x400008bc;
__umodsi3 = 0x400008c0;
__unorddf2 = 0x400008c4;
__unordsf2 = 0x400008c8;


/***************************************
Group newlib
***************************************/

/* Functions */
memset = 0x40000354;
memcpy = 0x40000358;
memmove = 0x4000035c;
memcmp = 0x40000360;
strcpy = 0x40000364;
strncpy = 0x40000368;
strcmp = 0x4000036c;
strncmp = 0x40000370;
strlen = 0x40000374;
strstr = 0x40000378;
bzero = 0x4000037c;
isalnum = 0x40000388;
isalpha = 0x4000038c;
isascii = 0x40000390;
isblank = 0x40000394;
iscntrl = 0x40000398;
isdigit = 0x4000039c;
islower = 0x400003a0;
isgraph = 0x400003a4;
isprint = 0x400003a8;
ispunct = 0x400003ac;
isspace = 0x400003b0;
isupper = 0x400003b4;
toupper = 0x400003b8;
tolower = 0x400003bc;
toascii = 0x400003c0;
memccpy = 0x400003c4;
memchr = 0x400003c8;
memrchr = 0x400003cc;
strcasecmp = 0x400003d0;
strcasestr = 0x400003d4;
strcat = 0x400003d8;
strdup = 0x400003dc;
strchr = 0x400003e0;
strcspn = 0x400003e4;
strcoll = 0x400003e8;
strlcat = 0x400003ec;
strlcpy = 0x400003f0;
strlwr = 0x400003f4;
strncasecmp = 0x400003f8;
strncat = 0x400003fc;
strndup = 0x40000400;
strnlen = 0x40000404;
strrchr = 0x40000408;
strsep = 0x4000040c;
strspn = 0x40000410;
strtok_r = 0x40000414;
strupr = 0x40000418;
longjmp = 0x4000041c;
setjmp = 0x40000420;
abs = 0x40000424;
div = 0x40000428;
labs = 0x4000042c;
ldiv = 0x40000430;
qsort = 0x40000434;
rand_r = 0x40000438;
rand = 0x4000043c;
srand = 0x40000440;
utoa = 0x40000444;
itoa = 0x40000448;
atoi = 0x4000044c;
atol = 0x40000450;
strtol = 0x40000454;
strtoul = 0x40000458;

uart_tx_one_char = 0x40000068;
uart_rx_one_char = 0x40000070;
uart_rx_one_char_block = 0x40000074;
uart_tx_flush = 0x40000080;

/***************************************
Group gpio
***************************************/

/* Functions */
gpio_input_get = 0x4000059c;
gpio_matrix_in = 0x400005a0;
gpio_matrix_out = 0x400005a4;
gpio_output_disable = 0x400005a8;
gpio_output_enable = 0x400005ac;
gpio_output_set = 0x400005b0;
gpio_pad_hold = 0x400005b4;
gpio_pad_input_disable = 0x400005b8;
gpio_pad_input_enable = 0x400005bc;
gpio_pad_pulldown = 0x400005c0;
gpio_pad_pullup = 0x400005c4;
gpio_pad_select_gpio = 0x400005c8;
gpio_pad_set_drv = 0x400005cc;
gpio_pad_unhold = 0x400005d0;
gpio_pin_wakeup_disable = 0x400005d4;
gpio_pin_wakeup_enable = 0x400005d8;
gpio_bypass_matrix_in = 0x400005dc;
