	;; This is very much the same as _switch_to_task, but we used iret
	;; and switch to ring3.
	[global _switch_to_user_task]
	;; _switch_to_user_task(uint page_directory, uint eip, uint ebp, uint esp)
_switch_to_user_task:			; (page_directory, eip, ebp, esp)
	add esp, 4					; We don't care about the return address

	pop ecx 					; Page directory
	pop eax 					; eip
	pop ebp
	pop ebx						; esp

	mov dx, 0x23				; User mode data segment
	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx
	
	mov cr3, ecx 				; Set page directory

	push 0x23
	push ebx 					; esp

	sti
	jmp eax 					; Jump back to code

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
