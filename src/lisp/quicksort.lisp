;;; Quicksort

(defun quicksort (l)
  (if (not l)
      l
      (let1 (rest (cdr l))
        (append
         (quicksort (remove-if-not
                     (lambda (x)
                  	   (< x (car l)))
                  	 rest))

         (list (car l))

         (quicksort (remove-if-not
                     (lambda (x)
                  	   (> x (car l)))
                  	 rest))))))

(defun main ()
  (print
   (quicksort
   	(list 12 3 4 1 6 8 10 5 14))))
