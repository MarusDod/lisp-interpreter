(defun fibonacci (x) (if (< x 3) 1 (+ (fibonacci (- x 1)) (fibonacci (- x 2)))))
(setf e (fibonacci 5))
(print e)
