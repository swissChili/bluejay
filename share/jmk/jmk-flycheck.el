(defun jmk-find-parent-makefile (p)
  "Find the parent makefile of file P."
  (let ((path (or p (expand-file-name "."))))
	(while (not (or (string= path "/")
					(file-exists-p (concat path "/Makefile"))))
	  (setq path (expand-file-name (concat path "/.."))))
	(unless (string= path "/")
	  (concat path "/Makefile"))))

(defun jmk-find-makefile (p)
  "Find the makefile for the source file P.
/a/b/src/hello.c -> /a/b/src/Makefile
/a/b/include/hello.h -> /a/b/src/Makefile
Returns nil if nothing can be found"
  (if-let ((makefile (jmk-find-parent-makefile p)))
	  makefile
	(jmk-find-parent-makefile (replace-regexp-in-string
							   (regexp-quote "include")
							   "src" p nil 'literal))))

(defun jmk-arguments-for (p)
  (if-let (path (jmk-find-makefile p))
	  (split-string (shell-command-to-string
					 (concat "make -f " path " show_cflags")) " ")
	nil))

(defun jmk-includes-for (p)
  (let* ((args (jmk-arguments-for p))
		 (includes (cl-remove-if-not (lambda (arg)
									   (string-prefix-p "-I" arg))
									 args))
		 (names (mapcar (lambda (arg)
						  (string-trim (substring arg 2)))
						includes)))
	(message "includes: %s" names)
	names))

