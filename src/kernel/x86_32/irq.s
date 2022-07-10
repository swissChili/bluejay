	[bits 32]

%macro IRQ 2
	[global irq%1]
irq%1:
	cli
	push 0		 				; Error code
	push %2						; Interrupt number
	jmp irq_common
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

	[extern irq_handler]
irq_common:
	pushad
	mov ax, ds					; Save data segment
	push eax

	mov ax, 0x10 				; New segments
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call irq_handler

	pop ebx						; Old data segment
	mov ds, bx
	mov es, bx
	mov fs, bx
	mov gs, bx

	popad
	add esp, 8
	sti
	iret

