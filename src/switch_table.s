	[section .text]
	[bits 32]

;;; TODO: remove useless frame pointer stuff
	
	[global load_page_directory]
load_page_directory:
	push ebp 					; Save frame pointer
	mov ebp, esp
	mov eax, [esp + 8]			; Pointer to directory
	mov cr3, eax
	mov esp, ebp
	pop ebp
	ret

	[global enable_paging]
enable_paging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	mov esp, ebp
	pop ebp
	ret
