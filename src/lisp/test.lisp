(defun add-two (a)
  (+ a 2))

(defun calls-gc (whatever)
  (print whatever)
  (gc))

(defmacro weird-identity (a)
  a)

(defmacro weird-const (a b)
  a)

(defun main ()
  (let1 (a (add-two 3))
    (print "a is")
    (print (weird-identity a))
    (print (weird-const a 4)))

  ; These allocations should be freed
  (list 12 34 56)
  (list "a" "b" "c" "d")
  
  ; But these should not
  (let1 (unused-but-bound (cons 5 6))
    (let1 (val '(a b c d e))
      (calls-gc val))))
