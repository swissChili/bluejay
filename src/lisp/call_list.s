;;; TODO: figure out if I need to do something special with the GC here.

	[bits 32]
	[global _call_list]
	[extern length]
	[extern elt]
	;;; This function should call it's first argument with the arguments from
	;;; the cons-list passed as its second argument.

	;;; _call_list(function pointer, cons list)
_call_list:
	push ebp
	mov ebp, esp

	mov edi, [ebp + 12]						; Cons list
	
	push edi
	call length								; Length of cons list in eax
	add esp, 4

	mov ecx, eax							; Store length in counter

	;; Reserve space for all the stack items
	shl eax, 2
	sub esp, eax

	mov esi, esp							; Pointer to top of stack

	;; Skip all of this if there are no arguments
	cmp ecx, 0
	je .done

.loop:
	;; Get the previous item. At the start ecx = the length so to get the last
	;; index we need to subtract 1
	dec ecx

	push ecx
	push edi
	call elt
	add esp, 4
	pop ecx									; This is a scratch register, remember

	;; We now have the ecx-th item in eax
	;; Remember esi is the top of the stack area reserved, so 
	mov [esi + 4 * ecx], eax

	jcxz .done
	jmp .loop

.done:
	mov ebx, [ebp + 8]						; Function pointer
	call ebx

	mov esp, ebp
	pop ebp
	ret
