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

(defun not (val)
  "Identical to NILP, returns T if VAL is NIL, NIL otherwise."
  (nilp val))

;; TODO: make tail recursive (for this `flet` would be nice)
(defun length (list)
  "Returns the length of LIST, or NIL if it is not a list"
  (if (nilp list)
    0
    (+ 1 (length (cdr list)))))

(defmacro when (cond & body)
  (list 'if cond
    (cons 'progn body)))

(defmacro unless (cond & body)
  (list 'if cond
    nil
    (cons 'progn body)))

(defun read ((stream nil))
  (if (not stream)
    (read-stdin)))

(defun funcall (fun & list)
  (apply fun list))

(defmacro flet1 (func & body)
  `(let1 (,(car func)
           (lambda ,@(cdr func)))
         ,@body))

(defmacro flet (funcs & body)
  (let1 (flet-
         (lambda (funcs body)
           (if funcs
               `(flet1 ,(car funcs)
                       ,(recurse (cdr funcs)
                                 body))
               `(progn ,@body))))
        (funcall flet- funcs body)))

(defmacro let (bindings & body)
  (flet ((let- (bindings body)
           (if bindings
               `(let1 ,(car bindings)
                      ,(recurse (cdr bindings)
                                body))
               `(progn ,@body)))))
  (funcall let- bindings body))

(load "list-functions.lisp")
(load "oop.lisp")
