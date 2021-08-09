(defun main ()
  (flet
      '((a () 123)
        (b () 456))
    (print (funcall b))))
