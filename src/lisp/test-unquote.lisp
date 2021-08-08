(defun numbers ()
  (list 1 2 3 4 5))

(defun main ()
  (print `(numbers are ,@(numbers) yeah)))
