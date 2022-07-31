	.equ STACK_SIZE, 8192

	.global _start

_start:
	csrr t0, mhartid
	slli t0, t0, 10 			# mhartid << 10
	la sp, stacks + STACK_SIZE	# set up space for hart stacks
	add sp, sp, t0				# sp += (mhartid << 10)

	## For now, only run on hart 0
	csrr a0, mhartid			# pass hart id to kmain
	bnez a0, .end
	j kmain

.end:
    wfi
    j .end

stacks:
    .skip STACK_SIZE * 4		# Allocate stack space for each hart
