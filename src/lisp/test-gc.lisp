(defun main ()
  ;; Allocate some garbage
  (let1 (used "this should NOT be freed")
	    (list 1 2 3 4)
        (list "this" "is" "a" "list")
        (gc)
        (print (list "Current allocations" "GC runs"))
        (print (gc-stats))))

;; Note: This should print that it is freeing both lists, but not the
;; string
