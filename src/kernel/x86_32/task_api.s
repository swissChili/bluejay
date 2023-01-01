	[global _switch_to_task]
_switch_to_task:				; (uint page_directory, struct
								; registers regs)
	add esp, 4					; We don't care about return address
	pop eax
	mov cr3, eax				; Change page directories
	pop eax
	mov ds, ax 					; First is ds
	popad						; Then the rest of the registers
	add esp, 8					; Then IRQ # and error #
	iret						; And finally the saved state

	[global _get_cr3]
_get_cr3:
	mov eax, cr3
	ret
