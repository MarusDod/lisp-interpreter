(defun only-evens (lst)
    (if (null lst) nil
        (if (eq (% (car lst) 2) 0)
            (cons (car lst) (only-evens (cdr lst)))
            (only-evens (cdr lst))
        )
    )
)

