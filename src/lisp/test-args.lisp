; (load "../../lib/lisp/std/std.lisp")

(defun with-optional (required (optional 3))
  (+ required optional))

(defun takes-varargs (& rest)
  (print rest))

(defun main ()
  (print "hi")
  (takes-varargs 1 2 3 4))
