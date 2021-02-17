	[bits 32]
	[global gdt_flush]

gdt_flush:
	mov eax, [esp + 4]
	lgdt [eax]					; Load GDT

	mov ax, 0x10				; Offset of data segment
	mov ds, ax,
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp 0x08:.flush				; Implicitly reloads the code segment
.flush:
	ret
