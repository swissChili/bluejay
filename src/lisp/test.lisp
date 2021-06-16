(defun add-two (a)
  (+ a 2))

(defun main-old ()
  (if t
    (print (add-two (* 4 3)))
    (print (- 3 6))))

(defun main ()
  (let1 (a 3)
    (print "a is")
    (print a)))
