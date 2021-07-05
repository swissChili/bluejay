;;;; std.lisp -- Lisp standard library

(defun caar (val)
  (car (car val)))

(defun cadr (val)
  (car (cdr val)))
 
(defun caddr (val)
  (car (cdr (cdr val))))

(defun cadar (val)
  (car (cdr (car val))))

(defun caddar (val)
  (car (cdr (cdr (car val)))))

;; Instead of a function this is a macro for a slight performance increase
(defmacro not (val)
  (nilp val))

;; TODO: make tail recursive (for this `flet` would be nice)
(defun length (list)
  (if (nilp list)
    0
    (+ 1 (length (cdr list)))))
