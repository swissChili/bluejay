;;;; Bluejay system call client library implementation.

	;; System call numbers
	%include "sys.inc"

	[bits 32]
	[global sys_giveup]
sys_giveup:
	mov eax, SYS_GIVEUP
	int SYS_INT
	ret							; Will return here after the task is re-scheduled
