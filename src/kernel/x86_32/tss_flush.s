	[bits 32]
	[global tss_flush]
tss_flush:
	mov ax, 0x2b 				; 0x28 = offset of TSS, | 0b11 to make
								; it user-readable
	ltr ax						; Load task register
	ret
