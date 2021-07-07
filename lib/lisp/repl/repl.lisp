(defun repl ()
  (let1 (val (read))
    (when val
        (print (eval val))
        (repl))))

(defun main ()
  (print "Welcome to the Bluejay REPL!")
  (repl))
