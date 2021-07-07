(defun mapcar (fun list)
  (if list
      (cons (funcall fun (car list))
            (mapcar fun (cdr list)))
      nil))
