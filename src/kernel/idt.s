	[bits 32]
	[global idt_flush]

idt_flush:
	mov eax, [esp + 4]
	lidt [eax]
	ret

%macro ISRNOERR 1
	[global isr%1]
isr%1:
	cli
	push 0
	push %1
	jmp isr_common
%endmacro

%macro ISRERR 1
	[global isr%1]
isr%1:
	cli
	push byte %1
	jmp isr_common
%endmacro

ISRNOERR 0
ISRNOERR 1
ISRNOERR 2
ISRNOERR 3
ISRNOERR 4
ISRNOERR 5
ISRNOERR 6
ISRNOERR 7
ISRERR 8
ISRNOERR 9
ISRERR 10
ISRERR 11
ISRERR 12
ISRERR 13
ISRERR 14
ISRNOERR 15
ISRNOERR 16
ISRNOERR 17
ISRNOERR 18
ISRNOERR 19
ISRNOERR 20
ISRNOERR 21
ISRNOERR 22
ISRNOERR 23
ISRNOERR 24
ISRNOERR 25
ISRNOERR 26
ISRNOERR 27
ISRNOERR 28
ISRNOERR 29
ISRNOERR 30
ISRNOERR 31

ISRNOERR 128
ISRNOERR 129

	[extern isr_handler]
isr_common:
	pusha						; Save all registers

	mov ax, ds					; Save data segment
	push eax

	mov ax, 0x10 				; New segments
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	call isr_handler

	pop eax						; Reset segments
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa
	add esp, 8 					; Passed arguments
	sti
	iret						; Return from interrupt
