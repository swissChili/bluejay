(defun two-plus-two ()
  (+ 2 2))

(defun hmm-main ()
  (print "64 / (2 + 2) =")
  (print (/ 64 (two-plus-two)))
  (print nil))

(defun main ()
  (if t
    (print 1)
    (print 2)))
