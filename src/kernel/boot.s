;;; GRUB Multiboot header, calls main() in main.c

	;; Some constants
MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002

MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

	;; Kernel memory start
KERNEL_VIRTUAL_BASE equ 0xC0000000
	;; Index in page directory
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

STACK_SIZE equ 0x4000


	[section .data align = 0x1000]
page_directory:	
	dd 0b010000011		; Identity map first 4 megs
	times (KERNEL_PAGE_NUMBER - 1) dd 0
	dd 0b010000011 ; Map kernel memory to zero page too 
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

	[bits 32]
	[section .text]
	[global mboot]
	[extern code]
	[extern bss]
	[extern end]
mboot:
	dd  MBOOT_HEADER_MAGIC ; This tells GRUB to start executing here:
	dd  MBOOT_HEADER_FLAGS
	dd  MBOOT_CHECKSUM
   
	dd  mboot                   ; Current location
	dd  code                    ; .text section
	dd  bss                     ; End of .data
	dd  end                     ; End of kernel
	dd  start

	[global start]
	[extern kmain]				; C code

start equ (_start - KERNEL_VIRTUAL_BASE)
	
_start:
	mov ecx, (page_directory - KERNEL_VIRTUAL_BASE) ; Physical address
	mov cr3, ecx

	mov ecx, cr4
	or ecx, 0x00000010			; Enable 4 meg pages
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000			; Enable paging
	mov cr0, ecx

	lea ecx, [in_higher_half]	; Long jump into high memory
	jmp ecx

in_higher_half:	
	mov dword [page_directory], 0
	invlpg [0]					; Clear identity mapping

	mov esp, (stack + STACK_SIZE)
	
	push ebx					; Holds multiboot header location, PHYSICAL addr
	call kmain
	jmp $


	[section .bss align = 32]
stack:
	resb STACK_SIZE
