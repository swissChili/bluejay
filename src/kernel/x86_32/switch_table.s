	[section .text]
	[bits 32]

	[global load_page_directory]
load_page_directory:
	mov ecx, [esp + 4]			; Pointer to directory
	mov cr3, ecx
	ret

	[global enable_paging]
enable_paging:
	mov ecx, cr0
	or eax, 0x80000000
	mov cr0, ecx
	ret
