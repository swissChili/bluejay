;;; scratch.s -- ideas for how code generation will work

	;; (defun lisp-function (A B)
	;;   (let ((local-var (whatever)))
	;;     (whatever local-var (something-else 4))))
	;; 
	;; This means the stack looks like this:
	;; B_value
	;; B_tag
	;; A_value
	;; A_tag
	;; result pointer
	;; return pointer     <---- esp 
lisp_function:
	push ebp					; Now ebp can be used as an offset
	mov ebp, esp
	sub esp, 16					; 1 var, 1 temporary

	;; Now, call `whatever' with no arguments
	;; For now we will do no register allocation, so don't even
	;; bother saving anything.
	lea eax, [ebp - 8]
	push eax					; Return address
	call whatever

	;; Now we need to evaluate (something-else 4) and store it in a
	;; temporary variable.
	;; First set up the literal 4
	push 4 						; The value
	push 0x20000000				; Type tag and length
	;; Then set up the return address
	lea eax, [ebp - 16] 
	push eax
	call something_else			; Result stored in temporary

	;; Next function: `whatever'
	push [ebp - 12]				; The temporary
	push [ebp - 16]
	push [ebp - 8]				; The variable
	push [ebp - 4]
	push [ebp + 4]				; The function's return address
	call whatever

	mov esp, ebp 				; Finally clean up
	pop ebp
	ret
