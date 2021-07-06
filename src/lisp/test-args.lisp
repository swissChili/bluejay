(defun with-optional (required (optional 3))
  (+ required optional))

(defun main ()
  (when t
    (print (with-optional 2))
    (print (with-optional 2 4))))
