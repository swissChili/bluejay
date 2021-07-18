	[bits 32]
	[extern _do_switch_task]
	[global switch_task]
switch_task:	
	pusha 						; Save everything
	push esp					; Arguments for _do_switch_task(eip, ebp, esp)
	push ebp
	push .after
	call _do_switch_task
.after:
	;; add esp, 12 				; Clear the arguments
	popa						; Reset everything
	xor eax, eax				; Return 0
	ret

	[global _switch_to_task]
	;; _switch_to_task(uint page_directory, uint eip, uint ebp, uint esp)
_switch_to_task:				; (page_directory, eip, ebp, esp)
	add esp, 4					; We don't care about the return address

	pop ecx 					; Page directory
	pop eax 					; eip
	pop ebp
	pop ebx						; esp
	
	mov esp, ebx 				; Reset old stack

	mov cr3, ecx 				; Set page directory
	sti
	jmp eax 					; Jump back to code
