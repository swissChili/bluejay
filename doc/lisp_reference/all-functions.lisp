;; Prints all defined Lisp functions
(defun main ()
  (mapcar (lambda (fun)
			(print fun))
		  (env-functions +current-env+)))
