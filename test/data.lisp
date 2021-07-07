(defun fact (x) (if (eq x 1) 1 (* x (fact (- x 1)))))
(setf q (fact 4))
(print q)
