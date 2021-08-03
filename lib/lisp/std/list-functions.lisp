(defun mapcar (fun list)
  "Maps over the cars of `list` by running `fun` on each one.
List must be a cons-list, other sequences are not supported."
  (if list
      (cons (funcall fun (car list))
            (mapcar fun (cdr list)))
      nil))

(defun remove-if (predicate list)
  "Returns a copy of `list` with elements satisfying `predicate`
removed."
  (if (not list)
      nil
      (if (funcall predicate (car list))
          (remove-if predicate (cdr list))
          (cons (car list)
                (remove-if predicate (cdr list))))))

(defun remove-if-not (predicate list)
  "Returns a copy of `list` with elements not satisfying `predicate`
removed."
  (remove-if (lambda (val)
               (not (funcall predicate val)))
             list))

(defun reduce (fun list (initial-value nil))
  "Combines elements of `list` into one element using `fun`. `fun` must
accept two arguments and return the result of combining those
arguments. The first argument will be the result so far and the second
will be the n-th item of the list. For the first item of the list, the
result so far will be `initial-value`, or `nil` by default."
  (if (nilp list)
      initial-value 
	  (reduce fun (cdr list)
			  (funcall fun initial-value
					   (car list)))))
