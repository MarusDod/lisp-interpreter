(defun mysum (x)
    (if (null x) 
        0
        (+ (car x) (mysum (cdr x)))
    )
)
