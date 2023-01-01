	[section .text]
	[bits 32]

	[global load_page_directory]
load_page_directory:
	mov ecx, [esp + 4]			; Pointer to directory
	mov cr3, ecx
	ret

	[global invalidate_page]
invalidate_page: 				; void (void *page)
	mov eax, [esp + 4]
	invlpg [eax]
	ret
