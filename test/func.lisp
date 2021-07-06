;(defun filter (fn lst)
;  (if lst
;    (if (fn (car lst))
;        (cons (car lst) (filter fn (cdr lst)))
;        (filter fn (cdr lst))
;    )
;    nil
;  )
;)
;
;(defun mapcar (fn lst)
;  (if lst
;    (cons (fn (car lst)) (mapcar fn (cdr lst)))
;    nil
;  )
;)
;
;(defun reverse (lst)
; (if lst (cons (reverse (cdr lst)) (car lst)) nil)
;)
;
(defun get-pair (lst)
    (car (cdr lst))
)

(defun search-list (symb lst)
   (if lst
   (if (eq symb (car (car lst))) (get-pair (car lst)) (search-list symb (cdr lst))) 
   nil
   )
)






