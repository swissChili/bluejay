;;; GRUB Multiboot header, calls main() in main.c

MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_HEADER_MAGIC  equ 0x1BADB002

MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


	[bits 32]

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

start:
	push ebx					; Holds multiboot header location

	cli
	call kmain
	jmp $
