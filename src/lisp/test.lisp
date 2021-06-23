(defun add-two (a)
  (+ a 2))

(defun calls-gc (whatever)
  (print whatever)
  (gc))

(defun main ()
  (let1 (a (add-two 3))
    (print "a is")
    (print a))

  ; These allocations should be freed
  (cons 12 (cons 34 (cons 45 nil)))
  
  ; But these should not
  (let1 (unused-but-bound (cons 5 6))
    (let1 (val (cons 1 (cons 2 (cons 3 nil))))
      (calls-gc val))))
