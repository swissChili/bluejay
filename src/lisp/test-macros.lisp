(defmacro weird-const (a b)
  a)

(defun main ()
  (let1 (var "this is var")
    (print (weird-const var 13))))
