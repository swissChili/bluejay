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
	popa						; Reset everything
	xor eax, eax				; Return 0
	ret

	[global _switch_to_task]
_switch_to_task:				; (page_directory, eip, ebp, esp)
	add esp, 4					; We don't care about the return address
	pop eax 					; Instruction pointer
	pop ebp 					; Frame pointer
	pop ebx 					; Stack pointer
	pop ecx 					; Page directory

	mov esp, ebx 				; Reset old stack

	mov cr3, ecx 				; Set page directory
	sti
	jmp eax 					; Jump back to code

	[extern _init_tasks]
	[global init_tasks]
init_tasks:
	lea eax, [esp + 4] 			; Stack pointer before call
	mov ebx, [esp] 				; Return address
	push ebx					; eip
	push ebp					; ebp
	push eax 					; esp
	call _init_tasks
	add esp, 12
	ret
