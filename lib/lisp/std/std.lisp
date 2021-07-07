;;;; std.lisp -- Lisp standard library

;; Boring utilities

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

;; /Boring utilitites



;; Instead of a function this is a macro for a slight performance increase
(defmacro not (val)
  (list 'nilp val))

;; TODO: make tail recursive (for this `flet` would be nice)
(defun length (list)
  "Returns the length of `list`, or `nil` if it is not a list"
  (if (nilp list)
    0
    (+ 1 (length (cdr list)))))

(defmacro when (cond & body)
  "Evaluate `body` when `cond` is truthy.
When `cond` is truthy, evaluates `body` in order, finally evaluating to
the final item."
  (list 'if cond
    (cons 'progn body)))

(defmacro unless (cond & body)
  "Evaluate `body` unless `cond` is truthy.
When `cond` is nil, evaluates `body` in order, finally evaluating to the
final item."
  (list 'if cond
    nil
    (cons 'progn body)))

(defun read ((stream nil))
  (if (not stream)
    (read-stdin)))

(print "Loading list functions")
(print (load "list-functions.lisp"))
