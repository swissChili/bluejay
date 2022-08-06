;;; oop.lisp

;; TODO
(defmacro defclass (name members)
  "(defclass person (name age (occupation nil)))"
  (let ((makefn-name (string->symbol (concat "make-" (symbol->string name))))
        (class-sym (gensym))
        (nmemb (length members)))
    `(defun ,makefn-name ,members
       (let ((,class-sym (make-class ,name ,nmemb)))
         ,class-sym))))
