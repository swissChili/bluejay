(defun mapcar (func list)
  (if list
    (cons (apply func (list (car list)))
          (mapcar func (cdr list)))
    nil))

(defun double (n)
  (+ n n))

(defun main ()
  (print (mapcar #'double
                 (list 1 2 3 4 5))))
