(defun main ()
  (print (reduce #'+
				 (list 1 2 3 4)
				 0)))
